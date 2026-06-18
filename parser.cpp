/*
 * 
 * This is a simple parser for scicalc
 * 
 * Friedrich Feichtinger, 09.09.2012
 * 
 */

#include "parser.h"
#include "scanner.h"
#include "variables.h"
#include "parseException.h"
#include "print.h"
#include "scicalc.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <QList>
#include <QDebug>
#include <Eigen/Dense>

namespace
{
	typedef Value::Number Number;

	Number cleanNumber(Number value);

	bool isZero(Number value)
	{
		return value.real()==0 && value.imag()==0;
	}

	bool isReal(Number value)
	{
		return value.imag()==0;
	}

	bool hasNaN(Number value)
	{
		return std::isnan(value.real()) || std::isnan(value.imag());
	}

	bool numberLess(Number left, Number right)
	{
		long double leftAbs=std::abs(left);
		long double rightAbs=std::abs(right);
		if(leftAbs!=rightAbs)
		{
			return leftAbs<rightAbs;
		}
		return std::arg(left)<std::arg(right);
	}

	long double requireRealNumber(Number value, QString context)
	{
		value=cleanNumber(value);
		if(!isReal(value))
		{
			throw ParseException(context + " expects real values");
		}
		return value.real();
	}

	Number fnSqrt(Number value) { return std::sqrt(value); }
	Number fnExp(Number value) { return std::exp(value); }
	Number fnLog(Number value) { return std::log(value); }
	Number fnLog10(Number value) { return std::log(value)/std::log(Number(10, 0)); }
	Number fnCeil(Number value) { return Number(ceill(value.real()), ceill(value.imag())); }
	Number fnFloor(Number value) { return Number(floorl(value.real()), floorl(value.imag())); }
	Number fnRound(Number value) { return Number(roundl(value.real()), roundl(value.imag())); }
	Number fnSin(Number value) { return std::sin(value); }
	Number fnCos(Number value) { return std::cos(value); }
	Number fnTan(Number value) { return std::tan(value); }
	Number fnAsin(Number value) { return std::asin(value); }
	Number fnAcos(Number value) { return std::acos(value); }
	Number fnAtan(Number value) { return std::atan(value); }
	Number fnAbs(Number value) { return Number(std::abs(value), 0); }
	Number fnReal(Number value) { return Number(value.real(), 0); }
	Number fnImag(Number value) { return Number(value.imag(), 0); }
	Number fnConj(Number value) { return std::conj(value); }
	Number fnAngle(Number value) { return Number(std::arg(value), 0); }
	Number fnRad2Deg(Number value) { return value*Number(180/M_PI, 0); }
	Number fnAtan2(Number y, Number x)
	{
		return Number(atan2l(requireRealNumber(y, "atan2"), requireRealNumber(x, "atan2")), 0);
	}

	long double cleanPart(long double value)
	{
		if(fabsl(value)<1e-12L)
		{
			return 0;
		}

		long double rounded=roundl(value);
		if(fabsl(value-rounded)<1e-12L)
		{
			return rounded;
		}

		return value;
	}

	Number cleanNumber(Number value)
	{
		return Number(cleanPart(value.real()), cleanPart(value.imag()));
	}

	Eigen::MatrixXcd toEigen(Value value)
	{
		Eigen::MatrixXcd matrix(value.rows(), value.columns());
		for(int r=0; r<value.rows(); r++)
		{
			for(int c=0; c<value.columns(); c++)
			{
				Number cell=value.at(r, c);
				matrix(r, c)=std::complex<double>(static_cast<double>(cell.real()), static_cast<double>(cell.imag()));
			}
		}
		return matrix;
	}

	Value fromEigen(const Eigen::MatrixXcd &matrix)
	{
		Value::Matrix result;
		for(int r=0; r<matrix.rows(); r++)
		{
			Value::Row row;
			for(int c=0; c<matrix.cols(); c++)
			{
				std::complex<double> cell=matrix(r, c);
				row.append(cleanNumber(Number(cell.real(), cell.imag())));
			}
			result.append(row);
		}
		return Value(result);
	}
}


Token* Parser::t=0;					// current token
Token* Parser::la=0;				// look-ahead token

Token::Kind Parser::sym=Token::none;			// kind of look-ahead-token
QString Parser::assignedVariable;


QString Parser::parse()
{
	//qDebug("start parsing");
	Value value;
	QString output;
	assignedVariable.clear();

	scan();

	try
	{
		if(sym==Token::eof)
		{
			// empty input line. no input, no output
			check(Token::eof);
			output="";
		}
		else
		{
			if(VersionFunction(&output))
			{
				// output has already been produced by the special string function
			}
			else
			{
				value=Assignment();
				if(sym==Token::semicolon)
				{
					// no output when ';' at end of line
					check(Token::semicolon);
					output="";
					
					// accept unit, but ignore it
					if(sym==Token::unit)
					{
						check(Token::unit);
						if(!assignedVariable.isEmpty())
						{
							Variables::set(assignedVariable, value, t->string);
						}
					}
				}
				else
				{
					// append unit
					if(sym==Token::unit)
					{
						check(Token::unit);
						output=formatValue(value, t->string);
						if(!assignedVariable.isEmpty())
						{
							Variables::set(assignedVariable, value, t->string);
						}
					}
					else
					{
						output=formatValue(value);
					}
				}
				check(Token::eof);
			}
		}
	}
	catch (ParseException e)
	{
		output=QString("ERROR: " + e.text());
	}
	return output;
}

QStringList Parser::functionNames()
{
	return QStringList()
		<< "sqrt"
		<< "exp"
		<< "log"
		<< "ln"
		<< "log10"
		<< "ceil"
		<< "floor"
		<< "round"
		<< "inv"
		<< "sin"
		<< "cos"
		<< "tan"
		<< "asin"
		<< "acos"
		<< "atan"
		<< "atan2"
		<< "abs"
		<< "real"
		<< "imag"
		<< "conj"
		<< "angle"
		<< "rad2deg"
		<< "getVersion"
		<< "setDigits"
		<< "setTrailingZeros"
		<< "setAccounting"
		<< "sum"
		<< "cumsum"
		<< "prod"
		<< "cumprod"
		<< "min"
		<< "max"
		<< "mean"
		<< "median"
		<< "std"
		<< "var"
		<< "size"
		<< "length"
		<< "numel"
		<< "reshape"
		<< "zeros"
		<< "ones"
		<< "eye"
		<< "diag"
		<< "linspace"
		<< "logspace"
		<< "det"
		<< "trace"
		<< "rank"
		<< "norm"
		<< "dot"
		<< "cross"
		<< "eig"
		<< "diff"
		<< "sort"
		<< "unique"
		<< "gradient"
		<< "trapz"
		<< "unwrap";
}


Value Parser::Power()
{
	//qDebug("parse Power");
	Value value;

	if(sym==Token::ident)
	{
		check(Token::ident);
		if(sym==Token::lpar)
		{
			// function call
			value=Function();
		}
		else
		{
			// variable or constant
			value=Variables::getValue(t->string);
		}
	}
	else if(sym==Token::number)
	{
		check(Token::number);
		Value::Number number(t->value, 0);
		if(t->string=="imaginarySuffix" && sym==Token::ident && isImaginaryUnit(la->string))
		{
			check(Token::ident);
			value=Value(number*Value::Number(0, 1));
		}
		else
		{
			value=Value(number);
		}
	}
	else if(sym==Token::lpar)
	{
		check(Token::lpar);
		value=Range();
		check(Token::rpar);
	}
	else if(sym==Token::lsquare)
	{
		value=MatrixLiteral();
	}
	else
	{
		throw ParseException("unexpected symbol '"+symbol(la)+"'");
	}

	while(sym==Token::transpose)
	{
		check(Token::transpose);
		value=transpose(value);
	}

	return value;
}


Value Parser::Factor()
{
	//qDebug("parse Factor");
	Value value=Power();
	while(sym==Token::hat || sym==Token::dotHat)
	{
		Token::Kind op=sym;
		check(op);
		if(op==Token::hat)
		{
			value=power(value, Power());
		}
		else
		{
			value=elementWise(value, Power(), op);
		}
	}
	return value;
}

Value Parser::Parallel()
{
	//qDebug("parse Parallel");
	Value value=Factor();
	while(sym==Token::times || sym==Token::slash || sym==Token::dotTimes || sym==Token::dotSlash)
	{
		if(sym==Token::times)
		{
			check(Token::times);
			value=multiply(value, Factor());
		}
		else if(sym==Token::slash)
		{
			check(Token::slash);
			value=divide(value, Factor());
		}
		else
		{
			Token::Kind op=sym;
			check(op);
			value=elementWise(value, Factor(), op);
		}
	}
	return value;
}

Value Parser::Term()
{
	//qDebug("parse Term");
	Value sum=Parallel();

	if(sym==Token::parallel)
	{
		while(sym==Token::parallel)
		{
			check(Token::parallel);
			sum=parallel(sum, Parallel());
		}
	}

	return sum;
}


Value Parser::Expression()
{
	//qDebug("parse Expression");
	Value value;
	bool hasValue=false;

	do
	{
		int sign=1;
		if(sym==Token::plus)
		{
			check(Token::plus);
			sign=1;
		}
		else if(sym==Token::minus)
		{
			check(Token::minus);
			sign=(-1);
		}

		Value term=Term();
		if(sign<0)
		{
			term=negate(term);
		}

		if(!hasValue)
		{
			value=term;
			hasValue=true;
		}
		else
		{
			value=add(value, term);
		}
	}
	while(sym==Token::plus || sym==Token::minus);

	return value;
}

Value Parser::Range()
{
	Value start=Expression();
	if(sym!=Token::colon)
	{
		return start;
	}

	check(Token::colon);
	Value second=Expression();
	if(sym==Token::colon)
	{
		check(Token::colon);
		return range(start, second, Expression());
	}

	return range(start, second);
}


Value Parser::Assignment()
{
	//qDebug("parse Assingment");
	Value value;

	if(Scanner::peek()->kind==Token::assign)
	{
		// this is an assignment
		check(Token::ident);
		QString var=t->string;
		check(Token::assign);
		value=Range();

		if(var!="$")
		{
			Variables::set(var, value);
			assignedVariable=var;
		}
	}
	else
	{
		// this is a normal expression
		value=Range();
	}

	Variables::set("$", value);

	return value;
}


Value Parser::Function()
{
	//qDebug("parse Function");
	QString fun;
	QList<Value> args;
	Value value;
	int n=0;

	// remember function name
	fun=t->string;

	// parse argument list
	check(Token::lpar);
	if(sym!=Token::rpar)
	{
		args.append(Range());
		while(sym==Token::comma)
		{
			check(Token::comma);
			args.append(Range());
		}
	}
	check(Token::rpar);

	// select function and execute it
	if     (fun=="sqrt"){	n=1;	value=mapUnary(args.value(0), fnSqrt, fun);}
	else if(fun=="exp"){	n=1;	value=mapUnary(args.value(0), fnExp, fun);}
	else if(fun=="log"){	n=1;	value=mapUnary(args.value(0), fnLog, fun);}
	else if(fun=="ln"){		n=1;	value=mapUnary(args.value(0), fnLog, fun);}
	else if(fun=="log10"){	n=1;	value=mapUnary(args.value(0), fnLog10, fun);}
	else if(fun=="ceil"){	n=1;	value=mapUnary(args.value(0), fnCeil, fun);}
	else if(fun=="floor"){	n=1;	value=mapUnary(args.value(0), fnFloor, fun);}
	else if(fun=="round"){	n=1;	value=mapUnary(args.value(0), fnRound, fun);}
	else if(fun=="inv"){	n=1;	value=inverse(args.value(0));}
	else if(fun=="sin"){	n=1;	value=mapUnary(args.value(0), fnSin, fun);}
	else if(fun=="cos"){	n=1;	value=mapUnary(args.value(0), fnCos, fun);}
	else if(fun=="tan"){	n=1;	value=mapUnary(args.value(0), fnTan, fun);}
	else if(fun=="asin"){	n=1;	value=mapUnary(args.value(0), fnAsin, fun);}
	else if(fun=="acos"){	n=1;	value=mapUnary(args.value(0), fnAcos, fun);}
	else if(fun=="atan"){	n=1;	value=mapUnary(args.value(0), fnAtan, fun);}
	else if(fun=="atan2"){	n=2;	value=mapBinary(args.value(0), args.value(1), fnAtan2, fun);}
	else if(fun=="abs"){	n=1;	value=mapUnary(args.value(0), fnAbs, fun);}
	else if(fun=="real"){	n=1;	value=mapUnary(args.value(0), fnReal, fun);}
	else if(fun=="imag"){	n=1;	value=mapUnary(args.value(0), fnImag, fun);}
	else if(fun=="conj"){	n=1;	value=mapUnary(args.value(0), fnConj, fun);}
	else if(fun=="angle"){	n=1;	value=mapUnary(args.value(0), fnAngle, fun);}
	else if(fun=="rad2deg"){n=1;	value=mapUnary(args.value(0), fnRad2Deg, fun);}
	else if(fun=="sum" || fun=="prod" || fun=="min" || fun=="max" ||
		fun=="mean" || fun=="median" || fun=="std" || fun=="var")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=reduce(args.value(0), fun, dimension);
		n=args.size();
	}
	else if(fun=="cumsum" || fun=="cumprod")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=cumulative(args.value(0), fun, dimension);
		n=args.size();
	}
	else if(fun=="size")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		value=sizeFunction(args, fun);
		n=args.size();
	}
	else if(fun=="length"){	n=1;	value=Value(std::max(args.value(0).rows(), args.value(0).columns()));}
	else if(fun=="numel"){	n=1;	value=Value(args.value(0).rows()*args.value(0).columns());}
	else if(fun=="reshape")
	{
		n=3;
		value=reshape(args.value(0), requireInteger(args.value(1), fun), requireInteger(args.value(2), fun));
	}
	else if(fun=="zeros" || fun=="ones" || fun=="eye")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int rows=requireInteger(args.value(0), fun);
		int columns=args.size()==2 ? requireInteger(args.value(1), fun) : rows;
		if(fun=="zeros"){ value=zeros(rows, columns); }
		else if(fun=="ones"){ value=ones(rows, columns); }
		else { value=eye(rows, columns); }
		n=args.size();
	}
	else if(fun=="diag"){	n=1;	value=diag(args.value(0));}
	else if(fun=="linspace" || fun=="logspace")
	{
		if(args.size()<2 || args.size()>3)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int count=args.size()==3 ? requireInteger(args.value(2), fun) : 100;
		value=linspace(args.value(0), args.value(1), count, fun=="logspace");
		n=args.size();
	}
	else if(fun=="det"){	n=1;	value=determinant(args.value(0));}
	else if(fun=="trace"){	n=1;	value=trace(args.value(0));}
	else if(fun=="rank"){	n=1;	value=rank(args.value(0));}
	else if(fun=="norm"){	n=1;	value=norm(args.value(0));}
	else if(fun=="dot"){	n=2;	value=dot(args.value(0), args.value(1));}
	else if(fun=="cross"){	n=2;	value=cross(args.value(0), args.value(1));}
	else if(fun=="eig"){	n=1;	value=eig(args.value(0));}
	else if(fun=="diff")
	{
		if(args.size()<1 || args.size()>3)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int order=args.size()>=2 ? requireInteger(args.value(1), fun) : 1;
		int dimension=args.size()==3 ? dimensionArgument(args.value(2), fun) : firstNonSingletonDimension(args.value(0));
		value=diff(args.value(0), order, dimension);
		n=args.size();
	}
	else if(fun=="sort")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=sort(args.value(0), dimension);
		n=args.size();
	}
	else if(fun=="unique"){	n=1;	value=unique(args.value(0));}
	else if(fun=="gradient")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=gradient(args.value(0), dimension);
		n=args.size();
	}
	else if(fun=="trapz")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=trapz(args.value(0), dimension);
		n=args.size();
	}
	else if(fun=="unwrap")
	{
		if(args.size()<1 || args.size()>2)
		{
			throw ParseException("invalid number of arguments for function '"+fun+"'");
		}
		int dimension=args.size()==2 ? dimensionArgument(args.value(1), fun) : firstNonSingletonDimension(args.value(0));
		value=unwrap(args.value(0), dimension);
		n=args.size();
	}
	else if(fun=="setDigits")
	{
		n=1;
		long double arg=requireRealScalar(args.value(0), fun);
		int digits=static_cast<int>(arg);
		if(arg!=digits)
		{
			throw ParseException("setDigits expects an integer argument");
		}
		if(digits<1 || digits>15)
		{
			throw ParseException("setDigits expects values between 1 and 15");
		}
		if(!scicalc::app()->setTemporaryDigits(digits))
		{
			throw ParseException("setDigits can only be called once per script");
		}
		value=Value(digits);
	}
	else if(fun=="setTrailingZeros")
	{
		n=1;
		long double arg=requireRealScalar(args.value(0), fun);
		int flag=static_cast<int>(arg);
		if(arg!=flag || (flag!=0 && flag!=1))
		{
			throw ParseException("setTrailingZeros expects 0 or 1");
		}
		if(!scicalc::app()->setTemporaryTrailingZeros(flag==1))
		{
			throw ParseException("setTrailingZeros can only be called once per script");
		}
		value=Value(flag);
	}
	else if(fun=="setAccounting")
	{
		n=1;
		long double arg=requireRealScalar(args.value(0), fun);
		int flag=static_cast<int>(arg);
		if(arg!=flag || (flag!=0 && flag!=1))
		{
			throw ParseException("setAccounting expects 0 or 1");
		}
		if(!scicalc::app()->setTemporaryAccounting(flag==1))
		{
			throw ParseException("setAccounting can only be called once per script");
		}
		value=Value(flag);
	}
	else
	{
		throw ParseException("unknown function '"+fun+"'");
	}

	if(n!=args.size())
	{
		throw ParseException("invalid number of arguments for function '"+fun+"'");
	}

	if(value.isScalar() && hasNaN(value.scalar()))
	{
		throw ParseException("function '"+fun+"' returned NaN");
	}

	return(value);
}

bool Parser::VersionFunction(QString *output)
{
	if(sym!=Token::ident || la->string!="getVersion")
	{
		return false;
	}

	check(Token::ident);
	check(Token::lpar);
	check(Token::rpar);
	check(Token::eof);
	*output=scicalc::getVersion();
	return true;
}

Value Parser::MatrixLiteral()
{
	check(Token::lsquare);

	Value::Matrix matrix;
	Value::Row row;
	int columns=-1;

	while(sym!=Token::rsquare)
	{
		if(sym==Token::eof)
		{
			throw ParseException("expected ']' but received 'end of line'");
		}
		if(sym==Token::semicolon)
		{
			throw ParseException("empty matrix rows are not supported");
		}
		if(!isExpressionStart(sym))
		{
			throw ParseException("unexpected symbol '"+symbol(la)+"' in matrix literal");
		}

		Value element=Range();
		if(element.isScalar())
		{
			row.append(element.scalar());
		}
		else if(element.rows()==1)
		{
			for(int c=0; c<element.columns(); c++)
			{
				row.append(element.at(0, c));
			}
		}
		else
		{
			throw ParseException("matrix literal elements must be scalar values or row vectors");
		}

		if(sym==Token::comma)
		{
			check(Token::comma);
		}
		else if(sym==Token::semicolon)
		{
			check(Token::semicolon);
			if(columns<0)
			{
				columns=row.size();
			}
			else if(row.size()!=columns)
			{
				throw ParseException("all matrix rows must have the same length");
			}
			matrix.append(row);
			row.clear();
			if(sym==Token::rsquare)
			{
				throw ParseException("empty matrix rows are not supported");
			}
		}
		else if(sym==Token::rsquare)
		{
			// matrix ends after this row
		}
		else if(isExpressionStart(sym))
		{
			// Matlab-style whitespace separated columns. Whitespace is skipped by the scanner,
			// so the next expression starts immediately.
		}
		else
		{
			throw ParseException("expected ',', ';' or ']' but received '"+symbol(la)+"'");
		}
	}

	check(Token::rsquare);

	if(!row.isEmpty())
	{
		if(columns<0)
		{
			columns=row.size();
		}
		else if(row.size()!=columns)
		{
			throw ParseException("all matrix rows must have the same length");
		}
		matrix.append(row);
	}

	return Value(matrix);
}


Value::Number Parser::divide(Value::Number dividend, Value::Number divisor)
{
	if(!isZero(cleanNumber(divisor)))
	{
		return dividend/divisor;
	}

	throw ParseException("division by zero");
}

Value::Number Parser::requireScalar(Value value, QString context)
{
	if(!value.isScalar())
	{
		throw ParseException(context + " expects scalar values");
	}
	return value.scalar();
}

long double Parser::requireRealScalar(Value value, QString context)
{
	return requireRealNumber(requireScalar(value, context), context);
}

QString Parser::formatValue(Value value, QString unit)
{
	if(value.isScalar())
	{
		bool complex=!isReal(cleanNumber(value.scalar()));
		return Print::complexPrint(value.scalar(), complex && !unit.isEmpty()) + unit;
	}

	if(value.rows()==0 || value.columns()==0)
	{
		return "[]" + unit;
	}

	QList<QStringList> formattedRows;
	QList<int> columnWidths;
	for(int c=0; c<value.columns(); c++)
	{
		columnWidths.append(0);
	}

	for(int r=0; r<value.rows(); r++)
	{
		QStringList formattedColumns;
		for(int c=0; c<value.columns(); c++)
		{
			QString cell=Print::complexPrint(cleanNumber(value.at(r, c)));
			formattedColumns.append(cell);
			if(cell.length()>columnWidths[c])
			{
				columnWidths[c]=cell.length();
			}
		}
		formattedRows.append(formattedColumns);
	}

	if(value.rows()==1)
	{
		QStringList columns;
		for(int c=0; c<value.columns(); c++)
		{
			columns.append(formattedRows.at(0).at(c).rightJustified(columnWidths.at(c), ' '));
		}
		return "[" + columns.join("  ") + "]" + unit;
	}

	QStringList rows;
	for(int r=0; r<value.rows(); r++)
	{
		QStringList columns;
		for(int c=0; c<value.columns(); c++)
		{
			columns.append(formattedRows.at(r).at(c).rightJustified(columnWidths.at(c), ' '));
		}
		QString leftBracket="⎢";
		QString rightBracket="⎥";
		if(r==0)
		{
			leftBracket="⎡";
			rightBracket="⎤";
		}
		else if(r==value.rows()-1)
		{
			leftBracket="⎣";
			rightBracket="⎦";
		}
		rows.append(leftBracket + " " + columns.join("  ") + " " + rightBracket);
	}
	return rows.join("\n") + unit;
}

bool Parser::isExpressionStart(Token::Kind kind)
{
	return kind==Token::ident || kind==Token::number || kind==Token::lpar ||
		kind==Token::lsquare || kind==Token::plus || kind==Token::minus;
}

bool Parser::isImaginaryUnit(QString name)
{
	return name=="i" || name=="j";
}

void Parser::requireSameSize(Value left, Value right, QString op)
{
	if(left.rows()!=right.rows() || left.columns()!=right.columns())
	{
		throw ParseException("matrix dimensions must agree for '" + op + "'");
	}
}

Value Parser::elementWise(Value left, Value right, Token::Kind op)
{
	if(left.isScalar() && right.isScalar())
	{
		switch(op)
		{
			case Token::plus:		return Value(cleanNumber(left.scalar() + right.scalar()));
			case Token::minus:		return Value(cleanNumber(left.scalar() - right.scalar()));
			case Token::times:
			case Token::dotTimes:	return Value(cleanNumber(left.scalar() * right.scalar()));
			case Token::slash:
			case Token::dotSlash:	return Value(cleanNumber(divide(left.scalar(), right.scalar())));
			case Token::hat:
			case Token::dotHat:		return Value(cleanNumber(std::pow(left.scalar(), right.scalar())));
			default:				throw ParseException("unsupported element-wise operator");
		}
	}

	int rows=left.isScalar() ? right.rows() : left.rows();
	int columns=left.isScalar() ? right.columns() : left.columns();
	if(!left.isScalar() && !right.isScalar())
	{
		requireSameSize(left, right, symbol(op));
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			Value::Number l=left.isScalar() ? left.scalar() : left.at(r, c);
			Value::Number rr=right.isScalar() ? right.scalar() : right.at(r, c);
			switch(op)
			{
				case Token::plus:		row.append(cleanNumber(l + rr)); break;
				case Token::minus:		row.append(cleanNumber(l - rr)); break;
				case Token::times:
				case Token::dotTimes:	row.append(cleanNumber(l * rr)); break;
				case Token::slash:
				case Token::dotSlash:	row.append(cleanNumber(divide(l, rr))); break;
				case Token::hat:
				case Token::dotHat:		row.append(cleanNumber(std::pow(l, rr))); break;
				default:				throw ParseException("unsupported element-wise operator");
			}
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::add(Value left, Value right)
{
	return elementWise(left, right, Token::plus);
}

Value Parser::subtract(Value left, Value right)
{
	return elementWise(left, right, Token::minus);
}

Value Parser::multiply(Value left, Value right)
{
	if(left.isScalar() || right.isScalar())
	{
		return elementWise(left, right, Token::times);
	}

	if(left.columns()!=right.rows())
	{
		throw ParseException("matrix dimensions must agree for '*'");
	}

	Value::Matrix result;
	for(int r=0; r<left.rows(); r++)
	{
		Value::Row row;
		for(int c=0; c<right.columns(); c++)
		{
			Value::Number sum(0, 0);
			for(int k=0; k<left.columns(); k++)
			{
				sum+=left.at(r, k)*right.at(k, c);
			}
			row.append(cleanNumber(sum));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::divide(Value left, Value right)
{
	if(right.isScalar())
	{
		return elementWise(left, right, Token::slash);
	}
	return multiply(left, inverse(right));
}

Value Parser::power(Value left, Value right)
{
	if(left.isScalar() && right.isScalar())
	{
		return Value(cleanNumber(std::pow(left.scalar(), right.scalar())));
	}
	throw ParseException("matrix power is not implemented; use '.^' for element-wise powers");
}

Value Parser::parallel(Value left, Value right)
{
	Value::Number l=requireScalar(left, "parallel operator");
	Value::Number r=requireScalar(right, "parallel operator");
	return Value(divide(l*r, l+r));
}

Value Parser::range(Value start, Value end)
{
	return range(start, Value(1), end);
}

Value Parser::range(Value start, Value step, Value end)
{
	long double startValue=requireRealScalar(start, "range operator");
	long double stepValue=requireRealScalar(step, "range operator");
	long double endValue=requireRealScalar(end, "range operator");

	if(stepValue==0)
	{
		throw ParseException("range step must not be zero");
	}

	Value::Row row;
	if((stepValue>0 && startValue<=endValue) || (stepValue<0 && startValue>=endValue))
	{
		long double value=startValue;
		int guard=0;
		while((stepValue>0 && value<=endValue) || (stepValue<0 && value>=endValue))
		{
			row.append(value);
			value+=stepValue;
			guard++;
			if(guard>100000)
			{
				throw ParseException("range produced too many elements");
			}
		}
	}

	Value::Matrix matrix;
	matrix.append(row);
	return Value(matrix);
}

Value Parser::inverse(Value value)
{
	if(value.isScalar())
	{
		return Value(divide(Value::Number(1, 0), value.scalar()));
	}

	if(value.rows()!=value.columns())
	{
		throw ParseException("inv expects a square matrix");
	}
	if(value.rows()==0)
	{
		throw ParseException("inv expects a non-empty matrix");
	}

	Eigen::MatrixXcd matrix=toEigen(value);
	Eigen::FullPivLU<Eigen::MatrixXcd> decomposition(matrix);
	if(!decomposition.isInvertible())
	{
		throw ParseException("matrix is singular");
	}

	return fromEigen(matrix.inverse());
}

Value Parser::transpose(Value value)
{
	if(value.isScalar())
	{
		return Value(std::conj(value.scalar()));
	}

	Value::Matrix result;
	for(int c=0; c<value.columns(); c++)
	{
		Value::Row row;
		for(int r=0; r<value.rows(); r++)
		{
			row.append(std::conj(value.at(r, c)));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::negate(Value value)
{
	return elementWise(Value(-1), value, Token::times);
}

Value Parser::mapUnary(Value value, Value::Number (*function)(Value::Number), QString context)
{
	if(value.isScalar())
	{
		Value::Number result=cleanNumber(function(value.scalar()));
		if(hasNaN(result))
		{
			throw ParseException("function '"+context+"' returned NaN");
		}
		return Value(result);
	}

	Value::Matrix result;
	for(int r=0; r<value.rows(); r++)
	{
		Value::Row row;
		for(int c=0; c<value.columns(); c++)
		{
			Value::Number cell=cleanNumber(function(value.at(r, c)));
			if(hasNaN(cell))
			{
				throw ParseException("function '"+context+"' returned NaN");
			}
			row.append(cell);
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::mapBinary(Value left, Value right, Value::Number (*function)(Value::Number, Value::Number), QString context)
{
	if(left.isScalar() && right.isScalar())
	{
		Value::Number result=cleanNumber(function(left.scalar(), right.scalar()));
		if(hasNaN(result))
		{
			throw ParseException("function '"+context+"' returned NaN");
		}
		return Value(result);
	}

	int rows=left.isScalar() ? right.rows() : left.rows();
	int columns=left.isScalar() ? right.columns() : left.columns();
	if(!left.isScalar() && !right.isScalar())
	{
		requireSameSize(left, right, context);
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			Value::Number l=left.isScalar() ? left.scalar() : left.at(r, c);
			Value::Number rr=right.isScalar() ? right.scalar() : right.at(r, c);
			Value::Number cell=cleanNumber(function(l, rr));
			if(hasNaN(cell))
			{
				throw ParseException("function '"+context+"' returned NaN");
			}
			row.append(cell);
		}
		result.append(row);
	}
	return Value(result);
}

int Parser::requireInteger(Value value, QString context)
{
	long double real=requireRealScalar(value, context);
	int integer=static_cast<int>(real);
	if(real!=integer)
	{
		throw ParseException(context + " expects integer values");
	}
	return integer;
}

int Parser::dimensionArgument(Value value, QString context)
{
	int dimension=requireInteger(value, context);
	if(dimension!=1 && dimension!=2)
	{
		throw ParseException(context + " dimension must be 1 or 2");
	}
	return dimension;
}

int Parser::firstNonSingletonDimension(Value value)
{
	return value.rows()>1 ? 1 : 2;
}

Value Parser::reduce(Value value, QString function, int dimension)
{
	if(value.isScalar())
	{
		if(function=="sum" || function=="prod" || function=="min" || function=="max" ||
			function=="mean" || function=="median")
		{
			return value;
		}
		if(function=="std" || function=="var")
		{
			return Value(0);
		}
	}

	int outer=dimension==1 ? value.columns() : value.rows();
	int inner=dimension==1 ? value.rows() : value.columns();
	Value::Row rowResult;
	Value::Matrix matrixResult;

	for(int o=0; o<outer; o++)
	{
		QVector<Value::Number> items;
		for(int i=0; i<inner; i++)
		{
			items.append(dimension==1 ? value.at(i, o) : value.at(o, i));
		}

		Value::Number result(0, 0);
		if(function=="sum" || function=="mean" || function=="std" || function=="var")
		{
			for(int i=0; i<items.size(); i++)
			{
				result+=items.at(i);
			}
			if(function=="mean")
			{
				result/=Value::Number(items.size(), 0);
			}
			else if(function=="std" || function=="var")
			{
				Value::Number mean=result/Value::Number(items.size(), 0);
				long double sumSquares=0;
				for(int i=0; i<items.size(); i++)
				{
					Value::Number delta=items.at(i)-mean;
					sumSquares+=std::norm(delta);
				}
				long double divisor=items.size()>1 ? items.size()-1 : 1;
				long double variance=sumSquares/divisor;
				result=Value::Number(function=="std" ? sqrtl(variance) : variance, 0);
			}
		}
		else if(function=="prod")
		{
			result=Value::Number(1, 0);
			for(int i=0; i<items.size(); i++)
			{
				result*=items.at(i);
			}
		}
		else if(function=="min" || function=="max" || function=="median")
		{
			std::sort(items.begin(), items.end(), numberLess);
			if(function=="min")
			{
				result=items.first();
			}
			else if(function=="max")
			{
				result=items.last();
			}
			else
			{
				int mid=items.size()/2;
				result=items.size()%2==1 ? items.at(mid) : (items.at(mid-1)+items.at(mid))/Value::Number(2, 0);
			}
		}
		else
		{
			throw ParseException("unknown reduction function '"+function+"'");
		}

		if(dimension==1)
		{
			rowResult.append(cleanNumber(result));
		}
		else
		{
			Value::Row row;
			row.append(cleanNumber(result));
			matrixResult.append(row);
		}
	}

	if(dimension==1)
	{
		matrixResult.append(rowResult);
	}
	if(matrixResult.size()==1 && matrixResult.at(0).size()==1)
	{
		return Value(matrixResult.at(0).at(0));
	}
	return Value(matrixResult);
}

Value Parser::cumulative(Value value, QString function, int dimension)
{
	if(value.isScalar())
	{
		return value;
	}

	Value::Matrix result=value.matrix();
	for(int r=0; r<value.rows(); r++)
	{
		result[r].resize(value.columns());
	}

	if(dimension==1)
	{
		for(int c=0; c<value.columns(); c++)
		{
			Value::Number acc=function=="cumprod" ? Value::Number(1, 0) : Value::Number(0, 0);
			for(int r=0; r<value.rows(); r++)
			{
				acc=function=="cumprod" ? acc*value.at(r, c) : acc+value.at(r, c);
				result[r][c]=cleanNumber(acc);
			}
		}
	}
	else
	{
		for(int r=0; r<value.rows(); r++)
		{
			Value::Number acc=function=="cumprod" ? Value::Number(1, 0) : Value::Number(0, 0);
			for(int c=0; c<value.columns(); c++)
			{
				acc=function=="cumprod" ? acc*value.at(r, c) : acc+value.at(r, c);
				result[r][c]=cleanNumber(acc);
			}
		}
	}
	return Value(result);
}

Value Parser::sizeFunction(QList<Value> args, QString context)
{
	Value value=args.value(0);
	if(args.size()==2)
	{
		int dimension=dimensionArgument(args.value(1), context);
		return Value(dimension==1 ? value.rows() : value.columns());
	}

	Value::Matrix result;
	Value::Row row;
	row.append(Value::Number(value.rows(), 0));
	row.append(Value::Number(value.columns(), 0));
	result.append(row);
	return Value(result);
}

Value Parser::reshape(Value value, int rows, int columns)
{
	if(rows<0 || columns<0)
	{
		throw ParseException("reshape dimensions must not be negative");
	}
	if(rows*columns!=value.rows()*value.columns())
	{
		throw ParseException("reshape dimensions must agree with number of elements");
	}

	QVector<Value::Number> elements;
	for(int c=0; c<value.columns(); c++)
	{
		for(int r=0; r<value.rows(); r++)
		{
			elements.append(value.at(r, c));
		}
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			row.append(elements.at(c*rows+r));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::zeros(int rows, int columns)
{
	if(rows<0 || columns<0)
	{
		throw ParseException("zeros dimensions must not be negative");
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			row.append(Value::Number(0, 0));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::ones(int rows, int columns)
{
	if(rows<0 || columns<0)
	{
		throw ParseException("ones dimensions must not be negative");
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			row.append(Value::Number(1, 0));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::eye(int rows, int columns)
{
	if(rows<0 || columns<0)
	{
		throw ParseException("eye dimensions must not be negative");
	}

	Value::Matrix result;
	for(int r=0; r<rows; r++)
	{
		Value::Row row;
		for(int c=0; c<columns; c++)
		{
			row.append(Value::Number(r==c ? 1 : 0, 0));
		}
		result.append(row);
	}
	return Value(result);
}

Value Parser::diag(Value value)
{
	if(value.rows()==1 || value.columns()==1)
	{
		int count=value.rows()*value.columns();
		Value::Matrix result;
		for(int r=0; r<count; r++)
		{
			Value::Row row;
			for(int c=0; c<count; c++)
			{
				Value::Number element(0, 0);
				if(r==c)
				{
					element=value.rows()==1 ? value.at(0, r) : value.at(r, 0);
				}
				row.append(element);
			}
			result.append(row);
		}
		return Value(result);
	}

	int count=std::min(value.rows(), value.columns());
	Value::Matrix result;
	for(int r=0; r<count; r++)
	{
		Value::Row row;
		row.append(value.at(r, r));
		result.append(row);
	}
	return Value(result);
}

Value Parser::linspace(Value start, Value end, int count, bool logarithmic)
{
	if(count<0)
	{
		throw ParseException("linspace count must not be negative");
	}
	Value::Matrix result;
	Value::Row row;
	Value::Number a=requireScalar(start, logarithmic ? "logspace" : "linspace");
	Value::Number b=requireScalar(end, logarithmic ? "logspace" : "linspace");
	for(int i=0; i<count; i++)
	{
		Value::Number value=count==1 ? b : a+(b-a)*Value::Number(i, 0)/Value::Number(count-1, 0);
		if(logarithmic)
		{
			value=std::pow(Value::Number(10, 0), value);
		}
		row.append(cleanNumber(value));
	}
	result.append(row);
	return Value(result);
}

Value Parser::determinant(Value value)
{
	if(value.rows()!=value.columns())
	{
		throw ParseException("det expects a square matrix");
	}
	std::complex<double> result=toEigen(value).determinant();
	return Value(cleanNumber(Number(result.real(), result.imag())));
}

Value Parser::trace(Value value)
{
	int count=std::min(value.rows(), value.columns());
	Value::Number result(0, 0);
	for(int i=0; i<count; i++)
	{
		result+=value.at(i, i);
	}
	return Value(cleanNumber(result));
}

Value Parser::rank(Value value)
{
	Eigen::MatrixXcd matrix=toEigen(value);
	Eigen::FullPivLU<Eigen::MatrixXcd> decomposition(matrix);
	return Value(decomposition.rank());
}

Value Parser::norm(Value value)
{
	if(value.rows()==1 || value.columns()==1)
	{
		long double sum=0;
		for(int r=0; r<value.rows(); r++)
		{
			for(int c=0; c<value.columns(); c++)
			{
				sum+=std::norm(value.at(r, c));
			}
		}
		return Value(sqrtl(sum));
	}

	Eigen::JacobiSVD<Eigen::MatrixXcd> svd(toEigen(value));
	return Value(svd.singularValues()(0));
}

Value Parser::dot(Value left, Value right)
{
	if(left.rows()*left.columns()!=right.rows()*right.columns())
	{
		throw ParseException("matrix dimensions must agree for 'dot'");
	}

	Value::Number result(0, 0);
	for(int i=0; i<left.rows()*left.columns(); i++)
	{
		int lr=i%left.rows();
		int lc=i/left.rows();
		int rr=i%right.rows();
		int rc=i/right.rows();
		result+=std::conj(left.at(lr, lc))*right.at(rr, rc);
	}
	return Value(cleanNumber(result));
}

Value Parser::cross(Value left, Value right)
{
	if(left.rows()*left.columns()!=3 || right.rows()*right.columns()!=3)
	{
		throw ParseException("cross expects 3-element vectors");
	}

	QVector<Value::Number> a;
	QVector<Value::Number> b;
	for(int i=0; i<3; i++)
	{
		a.append(left.rows()==1 ? left.at(0, i) : left.at(i, 0));
		b.append(right.rows()==1 ? right.at(0, i) : right.at(i, 0));
	}

	Value::Matrix result;
	Value::Row row;
	row.append(cleanNumber(a.at(1)*b.at(2)-a.at(2)*b.at(1)));
	row.append(cleanNumber(a.at(2)*b.at(0)-a.at(0)*b.at(2)));
	row.append(cleanNumber(a.at(0)*b.at(1)-a.at(1)*b.at(0)));
	result.append(row);
	return Value(result);
}

Value Parser::eig(Value value)
{
	if(value.rows()!=value.columns())
	{
		throw ParseException("eig expects a square matrix");
	}
	Eigen::ComplexEigenSolver<Eigen::MatrixXcd> solver(toEigen(value));
	Value::Matrix result;
	for(int i=0; i<solver.eigenvalues().rows(); i++)
	{
		Value::Row row;
		std::complex<double> eigenvalue=solver.eigenvalues()(i);
		row.append(cleanNumber(Value::Number(eigenvalue.real(), eigenvalue.imag())));
		result.append(row);
	}
	return Value(result);
}

Value Parser::diff(Value value, int order, int dimension)
{
	if(order<0)
	{
		throw ParseException("diff order must not be negative");
	}
	Value result=value;
	for(int n=0; n<order; n++)
	{
		int rows=dimension==1 ? std::max(result.rows()-1, 0) : result.rows();
		int columns=dimension==2 ? std::max(result.columns()-1, 0) : result.columns();
		Value::Matrix next;
		for(int r=0; r<rows; r++)
		{
			Value::Row row;
			for(int c=0; c<columns; c++)
			{
				Value::Number delta=dimension==1 ? result.at(r+1, c)-result.at(r, c) : result.at(r, c+1)-result.at(r, c);
				row.append(cleanNumber(delta));
			}
			next.append(row);
		}
		result=Value(next);
	}
	return result;
}

Value Parser::gradient(Value value, int dimension)
{
	if(value.isScalar())
	{
		return Value(0);
	}

	Value::Matrix result=value.matrix();
	for(int r=0; r<value.rows(); r++)
	{
		result[r].resize(value.columns());
	}

	if(dimension==1)
	{
		for(int c=0; c<value.columns(); c++)
		{
			for(int r=0; r<value.rows(); r++)
			{
				Value::Number delta;
				if(value.rows()==1){ delta=Value::Number(0, 0); }
				else if(r==0){ delta=value.at(1, c)-value.at(0, c); }
				else if(r==value.rows()-1){ delta=value.at(r, c)-value.at(r-1, c); }
				else { delta=(value.at(r+1, c)-value.at(r-1, c))/Value::Number(2, 0); }
				result[r][c]=cleanNumber(delta);
			}
		}
	}
	else
	{
		for(int r=0; r<value.rows(); r++)
		{
			for(int c=0; c<value.columns(); c++)
			{
				Value::Number delta;
				if(value.columns()==1){ delta=Value::Number(0, 0); }
				else if(c==0){ delta=value.at(r, 1)-value.at(r, 0); }
				else if(c==value.columns()-1){ delta=value.at(r, c)-value.at(r, c-1); }
				else { delta=(value.at(r, c+1)-value.at(r, c-1))/Value::Number(2, 0); }
				result[r][c]=cleanNumber(delta);
			}
		}
	}
	return Value(result);
}

Value Parser::sort(Value value, int dimension)
{
	if(value.isScalar())
	{
		return value;
	}

	Value::Matrix result=value.matrix();
	if(dimension==1)
	{
		for(int c=0; c<value.columns(); c++)
		{
			QVector<Value::Number> items;
			for(int r=0; r<value.rows(); r++){ items.append(value.at(r, c)); }
			std::sort(items.begin(), items.end(), numberLess);
			for(int r=0; r<value.rows(); r++){ result[r][c]=items.at(r); }
		}
	}
	else
	{
		for(int r=0; r<value.rows(); r++)
		{
			std::sort(result[r].begin(), result[r].end(), numberLess);
		}
	}
	return Value(result);
}

Value Parser::unique(Value value)
{
	QVector<Value::Number> items;
	for(int r=0; r<value.rows(); r++)
	{
		for(int c=0; c<value.columns(); c++)
		{
			items.append(value.at(r, c));
		}
	}
	std::sort(items.begin(), items.end(), numberLess);
	Value::Matrix result;
	Value::Row row;
	for(int i=0; i<items.size(); i++)
	{
		if(i==0 || items.at(i)!=items.at(i-1))
		{
			row.append(cleanNumber(items.at(i)));
		}
	}
	result.append(row);
	return Value(result);
}

Value Parser::trapz(Value value, int dimension)
{
	if(value.isScalar())
	{
		return Value(0);
	}

	int outer=dimension==1 ? value.columns() : value.rows();
	int inner=dimension==1 ? value.rows() : value.columns();
	Value::Matrix result;
	Value::Row rowResult;
	for(int o=0; o<outer; o++)
	{
		Value::Number sum(0, 0);
		for(int i=0; i<inner-1; i++)
		{
			Value::Number a=dimension==1 ? value.at(i, o) : value.at(o, i);
			Value::Number b=dimension==1 ? value.at(i+1, o) : value.at(o, i+1);
			sum+=(a+b)/Value::Number(2, 0);
		}
		if(dimension==1)
		{
			rowResult.append(cleanNumber(sum));
		}
		else
		{
			Value::Row row;
			row.append(cleanNumber(sum));
			result.append(row);
		}
	}
	if(dimension==1)
	{
		result.append(rowResult);
	}
	if(result.size()==1 && result.at(0).size()==1)
	{
		return Value(result.at(0).at(0));
	}
	return Value(result);
}

Value Parser::unwrap(Value value, int dimension)
{
	if(value.isScalar())
	{
		return Value(cleanNumber(Value::Number(requireRealNumber(value.scalar(), "unwrap"), 0)));
	}

	Value::Matrix result=value.matrix();
	for(int r=0; r<value.rows(); r++)
	{
		result[r].resize(value.columns());
	}

	int outer=dimension==1 ? value.columns() : value.rows();
	int inner=dimension==1 ? value.rows() : value.columns();
	for(int o=0; o<outer; o++)
	{
		long double offset=0;
		long double previous=0;
		for(int i=0; i<inner; i++)
		{
			int r=dimension==1 ? i : o;
			int c=dimension==1 ? o : i;
			long double current=requireRealNumber(value.at(r, c), "unwrap");
			if(i>0)
			{
				long double delta=current-previous;
				if(delta>M_PI){ offset-=2*M_PI; }
				else if(delta<-M_PI){ offset+=2*M_PI; }
			}
			result[r][c]=cleanNumber(Value::Number(current+offset, 0));
			previous=current;
		}
	}
	return Value(result);
}



void Parser::check(Token::Kind expected)
{
	//qDebug() << "check symbol" << symbol(expected);
	if(sym==expected)
	{
		scan();
	}
	else
	{
		throw ParseException("expected '"+symbol(expected)+"' but received '"+symbol(la)+"'");
	}
}


void Parser::scan()
{
	// delete old token and replace by new one from Scanner::next()
	if(t!=0)
	{
		delete t;
	}
	t=la;
	la=Scanner::next();
	sym=la->kind;
}


// returns the kind of token as QString
QString Parser::symbol(Token::Kind k)
{
	Token t;
	t.kind=k;
	t.string="name";
	return symbol(&t);
}

// returns the name of the tokens kind as QString
QString Parser::symbol(Token* t)
{
	QString string="";

	if(t->kind==Token::ident && !t->string.isEmpty())
	{
		string=t->string;
	}
	else
	{
		switch(t->kind)
		{
			case Token::none:		string="unknown symbol";	break;
			case Token::ident:		string="identifier";		break;
			case Token::unit:		string="unit";				break;
			case Token::number:		string="number";			break;
			case Token::assign:		string="=";					break;
			case Token::plus:		string="+";					break;
			case Token::minus:		string="-";					break;
			case Token::times:		string="*";					break;
			case Token::slash:		string="/";					break;
			case Token::hat:		string="^";					break;
			case Token::lpar:		string="(";					break;
			case Token::rpar:		string=")";					break;
			case Token::comma:		string=",";					break;
			case Token::parallel:	string="||";				break;
			case Token::semicolon:	string=";";					break;
			case Token::langle:		string="<";					break;
			case Token::rangle:		string=">";					break;
			case Token::lsquare:	string="[";					break;
			case Token::rsquare:	string="]";					break;
			case Token::colon:		string=":";					break;
			case Token::dotTimes:	string=".*";				break;
			case Token::dotSlash:	string="./";				break;
			case Token::dotHat:		string=".^";				break;
			case Token::transpose:	string="'";					break;
			case Token::eof:		string="end of line";		break;
		}
	}
	return string;
}
