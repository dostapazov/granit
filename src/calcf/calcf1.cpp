//#include <math.h>
//#include <ctype.h>
//#include <stdlib.h>
#pragma hdrstop
#include "calcf.hpp"



#ifndef _NO_NAMESPACE
//namespace KeRTL
//{
#endif

    typedef pair<string,math_func> m_pair;
	
	void __fastcall Init1(TMathFuncs & mf)
	{

		mf.insert(m_pair("+"    ,math_func (1,2,true,TMathFuncs::Add)));
		mf.insert(m_pair("-"    ,math_func (1,2,true,TMathFuncs::Sub)));
		mf.insert(m_pair("*"    ,math_func (2,2,true,TMathFuncs::Mul)));
		mf.insert(m_pair("/"    ,math_func (2,2,true,TMathFuncs::Div)));
		mf.insert(m_pair("%"    ,math_func (2,2,true,TMathFuncs::Remind)));
		mf.insert(m_pair("^"    ,math_func (3,2,true,TMathFuncs::Pow)));
                mf.insert(m_pair("<<"   ,math_func (1,2,true,TMathFuncs::LShift)));
                mf.insert(m_pair(">>"   ,math_func (1,2,true,TMathFuncs::RShift)));
                mf.insert(m_pair("lshift",math_func(1,2,false,TMathFuncs::LShift)));
                mf.insert(m_pair("rshift",math_func(1,2,false,TMathFuncs::RShift)));

	}

	void __fastcall Init2(TMathFuncs& mf)
	{
		mf.insert(m_pair("add"  ,math_func (3,-1,false,TMathFuncs::Add)));
		mf.insert(m_pair("sum"  ,math_func (3,-1,false,TMathFuncs::Add)));
		mf.insert(m_pair("sub"  ,math_func (3,2,false,TMathFuncs::Sub)));
		mf.insert(m_pair("mul"  ,math_func (3,-1,false,TMathFuncs::Mul)));
		mf.insert(m_pair("div"  ,math_func (3,2,false,TMathFuncs::Div)));
		mf.insert(m_pair("pow"  ,math_func (3,2,false,TMathFuncs::Pow)));
		mf.insert(m_pair("sqrt" ,math_func (3,1,false,TMathFuncs::Sqrt)));
		mf.insert(m_pair("ln"   ,math_func (3,1,false,TMathFuncs::Ln)));
		mf.insert(m_pair("lg"   ,math_func (3,1,false,TMathFuncs::Lg)));
		mf.insert(m_pair("log"  ,math_func (3,2,false,TMathFuncs::Log)));
		mf.insert(m_pair("rem"  ,math_func (3,2,false,TMathFuncs::Remind)));
                mf.insert(m_pair("pi"   ,math_func (3,-1,false,TMathFuncs::pi)));

	}

	void __fastcall Init3(TMathFuncs & mf)
	{
		mf.insert(m_pair("exp"  ,math_func (3,1,false,TMathFuncs::Exp)));
		mf.insert(m_pair("sin"  ,math_func (3,1,false,TMathFuncs::Sin)));
		mf.insert(m_pair("cos"  ,math_func (3,1,false,TMathFuncs::Cos)));
		mf.insert(m_pair("tan"  ,math_func (3,1,false,TMathFuncs::Tan)));
		mf.insert(m_pair("ctg"  ,math_func (3,1,false,TMathFuncs::Ctg)));

	}
	void __fastcall Init4(TMathFuncs& mf)
	{
		mf.insert(m_pair("asin" ,math_func (3,1,false,TMathFuncs::ASin)));
		mf.insert(m_pair("acos" ,math_func (3,1,false,TMathFuncs::ACos)));
		mf.insert(m_pair("atan" ,math_func (3,1,false,TMathFuncs::ATan)));
		mf.insert(m_pair("sinh" ,math_func (3,1,false,TMathFuncs::Sinh)));
		mf.insert(m_pair("cosh" ,math_func (3,1,false,TMathFuncs::Cosh)));

	}
	void __fastcall Init5(TMathFuncs & mf)
	{
		mf.insert(m_pair("round",math_func (3,2,false,TMathFuncs::Round)));
		mf.insert(m_pair("rad"  ,math_func (3,1,false,TMathFuncs::Rad)));
		mf.insert(m_pair("grad" ,math_func (3,1,false,TMathFuncs::Grad)));
		mf.insert(m_pair("fact" ,math_func (3,1,false,TMathFuncs::Fact)));
		mf.insert(m_pair("abs"  ,math_func (3,1,false,TMathFuncs::Abs )));

	}
	void __fastcall Init6(TMathFuncs & mf)
	{
		mf.insert(m_pair("b_and"  ,math_func (3,2,false,TMathFuncs::B_And )));
		mf.insert(m_pair("&"      ,math_func (1,2,true, TMathFuncs::B_And )));
		mf.insert(m_pair("l_and"  ,math_func (3,2,false,TMathFuncs::L_And )));
		mf.insert(m_pair("&&"     ,math_func (0,2,true ,TMathFuncs::L_And )));

	}
	void __fastcall Init7(TMathFuncs & mf)
	{

		mf.insert(m_pair("b_or"  ,math_func  (3,2,false,TMathFuncs::B_Or )));
		mf.insert(m_pair("l_or"  ,math_func  (3,2,false,TMathFuncs::L_Or )));
		mf.insert(m_pair("|"     ,math_func  (1,2,true, TMathFuncs::B_Or )));
		mf.insert(m_pair("||"    ,math_func  (0,2,true, TMathFuncs::L_Or )));
	}

	void __fastcall Init8(TMathFuncs & mf)
	{
		mf.insert(m_pair("b_not"  ,math_func (3,1,false,TMathFuncs::B_Not )));
		mf.insert(m_pair("l_not"  ,math_func (3,1,false,TMathFuncs::L_Not )));
		mf.insert(m_pair("b_xor"  ,math_func (3,2,false,TMathFuncs::B_Xor )));
		mf.insert(m_pair("=="     ,math_func (0,2,true, TMathFuncs::Equ )));
		mf.insert(m_pair("!="     ,math_func (0,2,true, TMathFuncs::NotEqu )));
	}
	void __fastcall Init9(TMathFuncs & mf)
	{
		mf.insert(m_pair("<"      ,math_func (0,2,true,TMathFuncs::Less )));
		mf.insert(m_pair(">"      ,math_func (0,2,true,TMathFuncs::Above)));
		mf.insert(m_pair("<="     ,math_func (0,2,true,TMathFuncs::LessEqu )));
		mf.insert(m_pair(">="     ,math_func (0,2,true,TMathFuncs::AboveEqu)));
                //mf.insert(m_pair("="      ,math_func (-1,2,true,TMathFuncs::Assign)));
                //mf.insert(m_pair("set"    ,math_func (3,2,false,TMathFuncs::Assign)));

	}

	TMathFuncs::TMathFuncs()
	{
                Init1(*this);
                Init2(*this);
                Init3(*this);
                Init4(*this);
                Init5(*this);
                Init6(*this);
                Init7(*this);
                Init8(*this);
                Init9(*this);

	}
#ifndef _NO_NAMESPACE
//namespace KeRTL
//}
#endif
