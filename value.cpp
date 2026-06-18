#include "value.h"

Value::Value()
{
	valueType=Scalar;
	scalarValue=Number(0, 0);
}

Value::Value(long double scalar)
{
	valueType=Scalar;
	scalarValue=Number(scalar, 0);
}

Value::Value(Number scalar)
{
	valueType=Scalar;
	scalarValue=scalar;
}

Value::Value(Matrix matrix)
{
	valueType=MatrixValue;
	scalarValue=Number(0, 0);
	matrixValue=matrix;
}

Value::Type Value::type() const
{
	return valueType;
}

bool Value::isScalar() const
{
	return valueType==Scalar;
}

bool Value::isMatrix() const
{
	return valueType==MatrixValue;
}

Value::Number Value::scalar() const
{
	return scalarValue;
}

Value::Matrix Value::matrix() const
{
	return matrixValue;
}

int Value::rows() const
{
	if(isScalar())
	{
		return 1;
	}
	return matrixValue.size();
}

int Value::columns() const
{
	if(isScalar())
	{
		return 1;
	}
	if(matrixValue.isEmpty())
	{
		return 0;
	}
	return matrixValue.at(0).size();
}

Value::Number Value::at(int row, int column) const
{
	if(isScalar())
	{
		return scalarValue;
	}
	return matrixValue.at(row).at(column);
}
