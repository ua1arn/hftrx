import re
import os

def scale_local_font_file(input_filename, output_filename, font_name, target_h=41):
    if not os.path.exists(input_filename):
        print(f"Ошибка: Локальный файл '{input_filename}' не найден!")
        return

    print(f"Обработка файла: {input_filename} ...")
    with open(input_filename, 'r', encoding='utf-8', errors='ignore') as f:
        text = f.read()

    # Очищаем от C++ однострочных (//...) и многострочных (/*...*/) комментариев
    text_clean = re.sub(r'//.*', '', text)
    text_clean = re.sub(r'/\*.*?\*/', '', text_clean, flags=re.DOTALL)

    # 1. Парсинг Bitmaps (Строгий поиск именно uint8_t массива)
    bitmap_match = re.search(r'const\s+uint8_t\s+(\w+)\s*(?:\[\])?\s*(?:PROGMEM)?\s*=\s*\{(.*?)\}\s*;', text_clean, flags=re.DOTALL)
    if not bitmap_match:
        print(f"  [!] Ошибка: В файле {input_filename} не найден массив Bitmaps")
        return
    bitmap_raw = bitmap_match.group(2)
    bitmap_bytes = [int(x.strip(), 16) for x in re.findall(r'0x[0-9a-fA-F]+', bitmap_raw)]

    # 2. Парсинг Glyphs (Строгий поиск именно GFXglyph массива)
    glyph_match = re.search(r'const\s+GFXglyph\s+(\w+)\s*(?:\[\])?\s*(?:PROGMEM)?\s*=\s*\{(.*?)\}\s*;', text_clean, flags=re.DOTALL)
    if not glyph_match:
        print(f"  [!] Ошибка: В файле {input_filename} не найден массив Glyphs")
        return
        
    extracted_array_name = glyph_match.group(1)
    glyph_raw = glyph_match.group(2)
    
    # Извлечение параметров структуры GFXglyph
    glyphs = []
    glyph_pattern = r'\{\s*([0-9a-fA-FXx]+|\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\}'
    for m in re.finditer(glyph_pattern, glyph_raw):
        glyphs.append({
            'offset': int(m.group(1), 16) if '0x' in m.group(1).lower() else int(m.group(1)),
            'w': int(m.group(2)),
            'h': int(m.group(3)),
            'xAdvance': int(m.group(4)),
            'xOffset': int(m.group(5)),
            'yOffset': int(m.group(6))
        })

    print(f"  -> Успешно найден массив глифов: {extracted_array_name} ({len(glyphs)} шт.)")

    # 3. Определение диапазона ASCII символов из структуры GFXfont в конце файла
    font_match = re.search(r'const\s+GFXfont\s+(\w+)\s*(?:PROGMEM)?\s*=\s*\{[^,]+,[^,]+,\s*(0x[0-9a-fA-F]+|\d+)\s*,\s*(0x[0-9a-fA-F]+|\d+)', text_clean, flags=re.DOTALL)
    first_char = 0x20
    last_char = 0x20 + len(glyphs) - 1
    if font_match:
        first_char = int(font_match.group(2), 16) if '0x' in font_match.group(2).lower() else int(font_match.group(2))
        last_char = int(font_match.group(3), 16) if '0x' in font_match.group(3).lower() else int(font_match.group(3))

    # Коэффициенты пропорционального масштабирования (-25% -> 0.75)
    orig_h = 54
    scale_y = target_h / orig_h
    scale_x = 0.75

    new_glyphs = []
    current_offset = 0

    # Расчет новых дескрипторов
    for i, g in enumerate(glyphs):
        if g['w'] == 0 or g['h'] == 0:
            new_glyphs.append({
                'offset': current_offset, 'w': 0, 'h': 0,
                'xAdvance': int(round(g['xAdvance'] * scale_x)),
                'xOffset': 0, 'yOffset': 0
            })
            continue

        new_w = int(round(g['w'] * scale_x))
        new_h = int(round(g['h'] * scale_y))
        if new_w == 0: new_w = 1
        if new_h == 0: new_h = 1

        new_glyphs.append({
            'offset': current_offset,
            'w': new_w,
            'h': new_h,
            'xAdvance': int(round(g['xAdvance'] * scale_x)),
            'xOffset': int(round(g['xOffset'] * scale_x)),
            'yOffset': int(round(g['yOffset'] * scale_y))
        })
        
        total_bits = new_w * new_h
        current_offset += (total_bits + 7) // 8

    # 4. Перепаковка битовых матриц (Nearest Neighbor) с посимвольным выравниванием по байтам
    aligned_bitmap_bytes = []
    for idx, g in enumerate(glyphs):
        if g['w'] == 0 or g['h'] == 0:
            continue
        
        src_bits = []
        start_bit = g['offset'] * 8
        for b_idx in range(start_bit, start_bit + (g['w'] * g['h'])):
            if (b_idx // 8) < len(bitmap_bytes):
                src_bits.append((bitmap_bytes[b_idx // 8] >> (7 - (b_idx % 8))) & 1)
            else:
                src_bits.append(0)
        
        while len(src_bits) < g['w'] * g['h']:
            src_bits.append(0)
            
        src_matrix = [src_bits[r * g['w'] : (r + 1) * g['w']] for r in range(g['h'])]
        
        nw, nh = new_glyphs[idx]['w'], new_glyphs[idx]['h']
        dbits = []
        for y in range(nh):
            sy = min(int(y / scale_y), g['h'] - 1)
            for x in range(nw):
                sx = min(int(x / scale_x), g['w'] - 1)
                dbits.append(src_matrix[sy][sx])
        
        while len(dbits) % 8 != 0:
            dbits.append(0)
            
        for i in range(0, len(dbits), 8):
            bval = 0
            for bi, bit in enumerate(dbits[i:i+8]):
                if bit: bval |= (1 << (7 - bi))
            aligned_bitmap_bytes.append(bval)

    # 5. Запись нового файла
    with open(output_filename, 'w', encoding='utf-8') as f:
        f.write(f'#ifndef __{font_name}_H_\n#define __{font_name}_H_\n\n')
        f.write('#include <Adafruit_GFX.h>\n\n')
        
        f.write(f'const uint8_t {font_name}Bitmaps[] PROGMEM = {{\n    ')
        for idx, b in enumerate(aligned_bitmap_bytes):
            f.write(f'0x{b:02X}, ')
            if (idx + 1) % 12 == 0:
                f.write('\n    ')
        f.write('\n};\n\n')

        f.write(f'const GFXglyph {font_name}Glyphs[] PROGMEM = {{\n')
        for i, ng in enumerate(new_glyphs):
            char_ascii = first_char + i
            char_repr = chr(char_ascii) if 32 <= char_ascii <= 126 else '?'
            f.write(f"    {{ {ng['offset']}, {ng['w']}, {ng['h']}, {ng['xAdvance']}, {ng['xOffset']}, {ng['yOffset']} }}, // 0x{char_ascii:02X} '{char_repr}'\n")
        f.write('};\n\n')

        f.write(f'const GFXfont {font_name} PROGMEM = {{\n')
        f.write(f'    (uint8_t *){font_name}Bitmaps,\n')
        f.write(f'    (GFXglyph *){font_name}Glyphs,\n')
        f.write(f'    0x{first_char:02X}, 0x{last_char:02X}, {target_h}\n')
        f.write('};\n\n#endif\n')
        
    print(f"  [+] Успешно сохранено: {output_filename}\n")

if __name__ == "__main__":
    # Обработка шрифтов Adafruit
    scale_local_font_file(
        input_filename="adafruit_28x54.h", 
        output_filename="adafruit_21x41.h", 
        font_name="adafruit_21x41"
    )
    
    scale_local_font_file(
        input_filename="adafruit_36x54.h", 
        output_filename="adafruit_27x41.h", 
        font_name="adafruit_27x41"
    )

    # Обработка шрифтов Century Gothic
    scale_local_font_file(
        input_filename="CenturyHothic_28x54.h", 
        output_filename="CenturyHothic_21x41.h", 
        font_name="CenturyHothic_21x41"
    )
    
    scale_local_font_file(
        input_filename="CenturyHothic_36x54.h", 
        output_filename="CenturyHothic_27x41.h", 
        font_name="CenturyHothic_27x41"
    )
