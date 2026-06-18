#ifndef VALUE_H
#define VALUE_H

#include <QVector>

class Value
{
public:
	typedef QVector<long double> Row;
	typedef QVector<Row> Matrix;

	enum Type { Scalar, MatrixValue };

	Value();
	explicit Value(long double scalar);
	explicit Value(Matrix matrix);

	Type type() const;
	bool isScalar() const;
	bool isMatrix() const;
	long double scalar() const;
	Matrix matrix() const;
	int rows() const;
	int columns() const;
	long double at(int row, int column) const;

private:
	Type valueType;
	long double scalarValue;
	Matrix matrixValue;
};

#endif
