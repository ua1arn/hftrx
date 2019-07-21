extern "C" { void debug_printf_P(const FLASHMEM char *format, ... ); }

class test
{
public:
	test()
	{
		debug_printf_P("*********** test");
	}
};


class test gltest;
