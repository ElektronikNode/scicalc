#ifndef VALUE_H
#define VALUE_H

#include <complex>
#include <QVector>

class Value
{
public:
	typedef std::complex<long double> Number;
	typedef QVector<Number> Row;
	typedef QVector<Row> Matrix;

	enum Type { Scalar, MatrixValue };

	Value();
	explicit Value(long double scalar);
	explicit Value(Number scalar);
	explicit Value(Matrix matrix);

	Type type() const;
	bool isScalar() const;
	bool isMatrix() const;
	Number scalar() const;
	Matrix matrix() const;
	int rows() const;
	int columns() const;
	Number at(int row, int column) const;

private:
	Type valueType;
	Number scalarValue;
	Matrix matrixValue;
};

#endif
