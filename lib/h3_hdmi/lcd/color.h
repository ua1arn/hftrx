#ifndef COLOR_H
#define COLOR_H

///---------------------------COLOR-------------------------------------------

#define RGB16(r,g,b)	(unsigned int)(((unsigned int)( ((r>>3)&0x1F)<<11)&0xF800)|(((unsigned int)((g>>2)&0x3f)<<5)&0x07E0)|((unsigned int)(b>>3)&0x001F))

/// color defines use formula: (rgb32>>8&0xf800)|(rgb32>>5&0x07e0)|(rgb32>>3&0x001f); and for blue (0xFF0000>>8&rgb32)|(0xFF0000>>5&rgb32)|(0xFF0000>>3&rgb32)
///http://forum.arduino.cc/index.php?topic=174989.0

#define  MAROON                     0x800000  ///RED
#define  DARK_RED                   0x8B0000
#define  BROWN                      0xA52A2A
#define  FIREBRICK                  0xB22222
#define  CRIMSON                    0xDC143C
#define  RED                        0xFF0000
#define  TOMATO                     0xFF6347
#define  CORAL                      0xFF7F50
#define  INDIAN_RED                 0xCD5C5C
#define  LIGHT_CORAL                0xF08080
#define  DARK_SALMON                0xE9967A
#define  SALMON                     0xFA8072
#define  LIGHT_SALMON               0xFFA07A
#define  ORANGE_RED                 0xFF4500
#define  DARK_ORANGE                0xFF8C00
#define  ORANGE                     0xFFA500
#define  GOLD                       0xFFD700
#define  DARK_GOLDEN_ROD            0xB8860B
#define  GOLDEN_ROD                 0xDAA520
#define  PALE_GOLDEN_ROD            0xEEE8AA
#define  DARK_KHAKI                 0xBDB76B
#define  KHAKI                      0xF0E68C
#define  OLIVE                      0x808000
#define  YELLOW                     0xFFFF00 ///YELLOW
#define  YELLOW_GREEN               0x9ACD32
#define  DARK_OLIVE_GREEN           0x556B2F
#define  OLIVE_DRAB                 0x6B8E23
#define  LAWN_GREEN                 0x7CFC00
#define  CHART_REUSE                0x7FFF00
#define  GREEN_YELLOW               0xADFF2F
#define  DARK_GREEN                 0x006400
#define  GREEN                      0x00FF00 ///GREEN
#define  FOREST_GREEN               0x228B22
#define  LIME                       0x00FF00
#define  LIME_GREEN                 0x32CD32
#define  LIGHT_GREEN                0x90EE90
#define  PALE_GREEN                 0x98FB98
#define  DARK_SEA_GREEN             0x8FBC8F
#define  MEDIUM_SPRING_GREEN        0x00FA9A
#define  SPRING_GREEN               0x00FF7F
#define  SEA_GREEN                  0x2E8B57
#define  MEDIUM_AQUA_MARINE         0x66CDAA
#define  MEDIUM_SEA_GREEN           0x3CB371
#define  LIGHT_SEA_GREEN            0x20B2AA
#define  DARK_SLATE_GRAY            0x2F4F4F
#define  TEAL                       0x008080
#define  DARK_CYAN                  0x008B8B
#define  AQUA                       0x00FFFF
#define  CYAN                       0x00FFFF
#define  LIGHT_CYAN                 0xE0FFFF
#define  DARK_TURQUOISE             0x00CED1
#define  TURQUOISE                  0x40E0D0
#define  MEDIUM_TURQUOISE           0x48D1CC
#define  PALE_TURQUOISE             0xAFEEEE
#define  AQUA_MARINE                0x7FFFD4
#define  POWDER_BLUE                0xB0E0E6
#define  CADET_BLUE                 0x5F9EA0
#define  STEEL_BLUE                 0x4682B4
#define  CORN_FLOWER_BLUE           0x6495ED
#define  DEEP_SKY_BLUE              0x00BFFF
#define  DODGER_BLUE                0x1E90FF
#define  LIGHT_BLUE                 0xADD8E6
#define  SKY_BLUE                   0x87CEEB
#define  LIGHT_SKY_BLUE             0x87CEFA
#define  MIDNIGHT_BLUE              0x191970
#define  NAVY                       0x0000FF
#define  DARK_BLUE                  0x00008B
#define  MEDIUM_BLUE                0x0000FF
#define  BLUE                       0x0000FF ///BLUE
#define  ROYAL_BLUE                 0x4169E1
#define  BLUE_VIOLET                0x8A2BE2
#define  INDIGO                     0x4B0082
#define  DARK_SLATE_BLUE            0x483D8B
#define  SLATE_BLUE                 0x6A5ACD
#define  MEDIUM_SLATE_BLUE          0x7B68EE
#define  MEDIUM_PURPLE              0x9370DB
#define  DARK_MAGENTA               0x8B008B
#define  DARK_VIOLET                0x9400D3
#define  DARK_ORCHID                0x9932CC
#define  MEDIUM_ORCHID              0xBA55D3
#define  PURPLE                     0x800080
#define  THISTLE                    0xD8BFD8
#define  PLUM                       0xDDA0DD
#define  VIOLET                     0xEE82EE
#define  MAGENTA                    0xFF00FF  ///MAGENTA
#define  ORCHID                     0xDA70D6
#define  MEDIUM_VIOLET_RED          0xC71585
#define  PALE_VIOLET_RED            0xDB7093
#define  DEEP_PINK                  0xFF1493
#define  HOT_PINK                   0xFF69B4
#define  LIGHT_PINK                 0xFFB6C1
#define  PINK                       0xFFC0CB  ///PINK
#define  ANTIQUE_WHITE              0xFAEBD7
#define  BEIGE                      0xF5F5DC
#define  BISQUE                     0xFFE4C4
#define  BLANCHED_ALMOND            0xFFEBCD
#define  WHEAT                      0xF5DEB3
#define  CORN_SILK                  0xFFF8DC
#define  LEMON_CHIFFON              0xFFFACD
#define  LIGHT_GOLDEN_ROD_YELLOW    0xFAFAD2
#define  LIGHT_YELLOW               0xFFFFE0
#define  SADDLE_BROWN               0x8B4513
#define  SIENNA                     0xA0522D
#define  CHOCOLATE                  0xD2691E
#define  PERU                       0xCD853F
#define  SANDY_BROWN                0xF4A460
#define  BURLY_WOOD                 0xDEB887
#define  TAN                        0xD2B48C
#define  ROSY_BROWN                 0xBC8F8F
#define  MOCCASIN                   0xFFE4B5
#define  NAVAJO_WHITE               0xFFDEAD
#define  PEACH_PUFF                 0xFFDAB9
#define  MISTY_ROSE                 0xFFE4E1
#define  LAVENDER_BLUSH             0xFFF0F5
#define  LINEN                      0xFAF0E6
#define  OLD_LACE                   0xFDF5E6
#define  PAPAYA_WHIP                0xFFEFD5
#define  SEA_SHELL                  0xFFF5EE
#define  MINT_CREAM                 0xF5FFFA
#define  SLATE_GRAY                 0x708090
#define  LIGHT_SLATE_GRY            0x778899
#define  LIGHT_STEEL_BLUE           0xB0C4DE
#define  LAVENDER                   0xE6E6FA
#define  FLORAL_WHITE               0xFFFAF0
#define  ALICE_BLUE                 0xF0F8FF
#define  GHOST_WHITE                0xF8F8FF
#define  HONEYDEW                   0xF0FFF0
#define  IVORY                      0xFFFFF0
#define  AZURE                      0xF0FFFF
#define  WHATE                      0xFFFAFA
#define  BLACK                      0x000000  ///BLACK
#define  STRONG_GREY                0x575757
#define  DIM_GREY                   0x696969
#define  GREY                       0x808080
#define  DARK_GREY                  0xA9A9A9
#define  SILVER                     0xC0C0C0
#define  LIGHT_GREY                 0xD3D3D3
#define  GAINSBORO                  0xDCDCDC
#define  WHITE_SMOKE                0xF5F5F5
#define  WHITE                      0xFFFFFF  ///WHITE


#endif
