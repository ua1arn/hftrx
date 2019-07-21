extern "C" { void debug_printf_P(const char *format, ... ); }

class test
{
public:
	test()
	{
		debug_printf_P("*********** test @%p\n", this);
	}
	~test()
	{
		debug_printf_P("----------- test @%p\n", this);
	}
};

class test2
{
public:
	test2()
	{
		debug_printf_P("*********** test2 @%p\n", this);
	}
};


class test2 gltest;


extern "C" { void ttt(void); }

void ttt()
{
	test t1;
	test t2;

}

