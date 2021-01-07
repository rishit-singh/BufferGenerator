#include <iostream>
#include <vector>
#include <GL/glew.h> 
#include "debug.h"

DebugTools::Debugger* Debug = new DebugTools::Debugger();

enum ObjectType
{
	VertexBuffer, 
	IndexBuffer
}; 

template<typename T>
bool IsElement(T val, std::vector<T> array)
{
	int size = array.size(); 

	for (int x = 0; x < size; x++)
		if (array.at(x) == val)
			return true; 
	
	return false; 
}

template<typename T> 
struct Matrix
{
	std::vector<T> VectorArray;
	int Stride; 

	Matrix() : VectorArray(std::vector<T>()), Stride(0)
	{
	}

	Matrix(T* array, int size, int stride) : VectorArray(std::vector<T>()), Stride(stride)
	{
		if ((size % stride) != 0)
			std::cout << "The row size can't divide the matrix array into rows of equal sizes, accessing some elements may lead to segmentation fault.";

		for (int x = 0; x < size; x++)
			this->VectorArray.push_back(array[x]); 
	}

	Matrix(std::vector<T> vectorArray, int stride) : VectorArray(vectorArray), Stride(stride)
	{
		if ((vectorArray.size() % stride) != 0)
			std::cout << "The row size can't divide the matrix array into rows of equal sizes, accessing some elements may lead to segmentation fault.";
	}
}; 

class MatrixOperations	//	contains static functions to work with matrix arrays
{
public:
	//	Checks if the provided row array's elements matches a the elements of other rows in the matrix array
	template<typename T>
	static bool IsRow(std::vector<T> rowVectorArray, Matrix<T> matrix)	
	{
		if (rowVectorArray.size() != matrix.Stride)
		{
			std::cout << "Invalid row vector size";

			return false;
		}

		for (int x = 0; x < (matrix.VectorArray.size() / matrix.Stride); x++)
			if (rowVectorArray.at(0) == matrix.VectorArray.at((matrix.Stride * 3)) && rowVectorArray.at(1) == matrix.VectorArray.at((matrix.Stride * 3) + 1) && rowVectorArray.at(2) == matrix.VectorArray.at((matrix.Stride * 3) + 2))
				return true;

		return false; 
	}

	template<typename T>
	static unsigned int AtMatrixRowIndex(std::vector<T> rowVectorArray, Matrix<T> matrix, std::vector<unsigned int> indices)	//	returns the index stored in the provided index array to which the provided row is matched
	{
		std::vector<unsigned int> IndexVectorArray = std::vector<unsigned int>();

		int size, indexArraySize = indices.size();
		unsigned int indexTemp; 
		
		if ((matrix.VectorArray.size() % rowVectorArray.size()) != 0)				
		{
			std::cout << "Invalid stride or row size.";

			return -1; 
		}

		// Debug->Log("Before loop."); 
		// Debug->Log(indexArraySize);

		for (int y = 0; y < indexArraySize; y++)
			try
			{	
				// Debug->Log(y); 
				indexTemp = (matrix.Stride * indices.at(y)); 

				// std::cout << "\nInitial matrix elements:";
				 
				for (int x = 0; x < 3; x++)
					Debug->Log(matrix.VectorArray.at(indexTemp + x));

				bool debugBool; 

				std::cout << "rowVectorArray.size(): "; 
				Debug->Log(rowVectorArray.size());

				if (debugBool = (rowVectorArray.at(0) == matrix.VectorArray.at(indexTemp = (matrix.Stride * indices.at(y)))  && rowVectorArray.at(1) == matrix.VectorArray.at(indexTemp + 1) && rowVectorArray.at(2) == matrix.VectorArray.at((indexTemp + 2))))
				{
					std::cout << "\n debugBool: " << debugBool; 
					// Debug->Log("Found at "); 

					return y;
				}
			}
			catch (std::out_of_range& e)
			{
				std::cout << e.what() << "\nIndex out of range.";

				return -1; 
			}

			// Debug->Log<unsigned int>(IndexVectorArray); 

		return -1; 
	}

	template<typename T>
	static bool IsDuplicate(std::vector<T> rowVectorArray, Matrix<T> matrix, std::vector<T> indices)
	{
		return (MatrixOperations::AtMatrixRowIndex<T>(rowVectorArray, matrix, indices) != -1);
	}
};

struct VertexData   //  Stores the index and vertex buffer arrays
{
	unsigned int VertexBufferObject, IndexBufferObject;

	VertexData() : VertexBufferObject(NULL), IndexBufferObject(NULL)   
	{
	}

	VertexData(unsigned int vertexBufferObject, unsigned int indexBufferObject) : VertexBufferObject(vertexBufferObject), IndexBufferObject(indexBufferObject)
	{
	}

	VertexData(std::vector<float> vertexVectorArray, std::vector<unsigned int> indexVectorArray)
	{
		unsigned int* bufferObjects = new unsigned int[2];

		for (int x = 0; x < 2; x++)
			glGenBuffers(1, &bufferObjects[x]);

		glBindBuffer(1, bufferObjects[0]); 

		glBufferData(GL_ARRAY_BUFFER, vertexVectorArray.size(),  vertexVectorArray.data(), GL_STATIC_DRAW); 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexVectorArray.size() * sizeof(unsigned int), indexVectorArray.data(), GL_STATIC_DRAW); 

		this->VertexBufferObject = bufferObjects[0];
		this->IndexBufferObject = bufferObjects[1];
	}
}; 

VertexData GetVertexData(Matrix<float> vertexMatrix)	//	Generates an index buffer array and an altered vertex buffer array		{
{	
	std::vector<float> VertexBufferArray = std::vector<float>(); 
	std::vector<unsigned int> IndexBufferArray = std::vector<unsigned int>(); 

	int size = NULL, indexTemp, matrixSize = vertexMatrix.VectorArray.size(); 

	if ((vertexMatrix.VectorArray.size() % vertexMatrix.Stride) > 0)
	{	
		std::cout << "The row length or st ride are invalid."; 

		return VertexData(); 
	}
	else 
		size = vertexMatrix.VectorArray.size() / vertexMatrix.Stride;


	// std::cout << '\n' << "MatrixSize: "
	// 	<< matrixSize;

	for (int x = 0; x < matrixSize; x += vertexMatrix.Stride)
	{
		if (!x)	
		{
			for (int y = 0; y < vertexMatrix.Stride; y++)
				VertexBufferArray.push_back(vertexMatrix.VectorArray.at(x + y)); 

 			IndexBufferArray.push_back(x); 
		}
	
		Debug->Log("x: ", x);  

		try 
		{
			if ((indexTemp = (MatrixOperations::AtMatrixRowIndex({ vertexMatrix.VectorArray.at(x), vertexMatrix.VectorArray.at(x + 1), vertexMatrix.VectorArray.at(x + 2) }, vertexMatrix, IndexBufferArray)) != -1))
			{					
				IndexBufferArray.push_back(IndexBufferArray.at(indexTemp));

				std::cout << "indexTemp:";
		
				Debug->Log(indexTemp); 

				std::cout << '\n'; 
			}
			else
			{			
				for (int y = 0; y < vertexMatrix.Stride; y++)
					VertexBufferArray.push_back(vertexMatrix.VectorArray.at(x + y)); 

				IndexBufferArray.push_back(indexTemp); 
			}
		}
		catch (std::exception& e)
		{
			Debug->Log(e.what());

			return VertexData();  
		}
	}
	
	return VertexData(VertexBufferArray, IndexBufferArray); 
}

int main()
{
	VertexData vertexData = GetVertexData(Matrix<float>({
		-0.5f, -0.5f, 0.0f, 
		0.5f, -0.5f, 0.0f, 
		0.0f, 0.5f, 0.0f, 

		-0.5f, -0.5f, 0.0f, 
		-0.5f, 0.5f, 0.0f, 
		0.0f, 0.5f, 0.0f
	}, 3)); 

	// int index = MatrixOperations::AtMatrixRowIndex(
	// 	{ -0.5f, -0.5f, 0.0f }, 

	// 	Matrix<float>( {
	// 		-0.5f, -0.5f, 0.0f, 
	// 		0.5f, -0.5f, 0.0f, 
	// 		0.0f, 0.5f, 0.0f, 

	// 		-0.5f, -0.5f, 0.0f, 
	// 		-0.5f, 0.5f, 0.0f, 
	// 		0.0f, 0.5f, 0.0f
	// 	}, 3), 

	// 	{ 3 }
	// );

	return 0; 
}
