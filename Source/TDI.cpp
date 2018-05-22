#include <stdio.h>
#include <C_Image.hpp>
#include <C_Matrix.hpp>
#include <C_General.hpp>

C_Image imagenEntrada;
C_Image imagenSalida;
C_Matrix matrizConvolucion;
char nombreFiltro[80];


int CalcularValorPixel(int x, int y, int i, int j, C_Matrix matrizConvolucion);
void ReajusteValores();

void matrizFiltroMedia();
void matrizFiltroPasoBajo();
void matrizFiltroPasoAlto3x3();
void matrizFiltroPasoAlto5x5();
void matrizFiltroGaussiano3x3();
void matrizFiltroGaussiano5x5();
void matrizFiltroLaplaciano3x3();
void matrizFiltroLaplaciano5x5();
void matrizFiltroSobelX();
void matrizFiltroSobelY();
void matrizFiltroPrewittY();
void matrizFiltroPrewittX();
// por desarrollar
//void matrizMediaPonderada();
//void matrizFiltroGausiano7x7();
//void matrizFiltroLaplaciano7x7();


int main(int argc, char **argv)
{

	printf("#   APLICACION DE FILTROS MEDIANTE CONVOLUCION   #\n");
	printf("--------------------------------------------------\n");

	//Lectura de la imagen de entrada (si el archivo no existe, vuelve a pedirtelo):
	char nombreSinFormato[80] = "";

	do {
		char nombreImagen[80] = "";

		//Primero limpia la variable imagenEntrada, por si hubiera más de un intento no interfiera el intento fallido anterior

		//(Por hacer)Imprimir tambien la ruta donde se buscaran las imagenes introducidas por nombre
		imagenEntrada.Free();
		printf("\n\nIntroduce el nombre de la imagen (sin incluir el formato '.bmp'):\n");
		scanf("%s", nombreSinFormato);

		strcat(nombreImagen, nombreSinFormato);
		strcat(nombreImagen, ".bmp");

		imagenEntrada.ReadBMP(nombreImagen);
	} while (imagenEntrada.Empty());


	int opcion = 0;
	do {
		printf("\n\n");
		printf("           FILTROS                      \n");
		printf("----------------------------------------\n");
		printf("   1. Filtro Media                      \n");
		printf("   2. Filtro Paso Bajo                  \n");
		printf("   3. Filtro Paso Alto                  \n");
		printf("   4. Filtro Paso Alto (5x5)            \n");
		printf("   5. Filtro Gaussiano                  \n");
		printf("   6. Filtro Gaussiano (5x5)            \n");
		printf("   7. Filtro Laplaciano                 \n");
		printf("   8. Filtro Laplaciano (5x5)           \n");
		printf("   9. Filtro Sobel X                    \n");
		printf("  10. Filtro Sobel Y                    \n");
		printf("  11. Filtro Prewitt X                  \n");
		printf("  12. Filtro Prewitt Y                  \n");
		printf("  13. Filtro Media Ponderado(prox.)     \n");
		printf("  14. Filtro Gaussiano (7x7)(prox.)     \n");
		printf("  15. Filtro Laplaciano (7x7)(prox.)    \n");
		printf("                                        \n");
		printf("  0. Salir del menu                     \n");
		

		printf("Introduzca el número del filtro a aplicar: ");
		scanf("%d", &opcion);
	} while (opcion < 0 || opcion > 12);


	//Se llama a un método u otro según el filtro para crear la matriz de convolución correspondiente.
	switch (opcion) {
	case 1:
		matrizFiltroMedia();
		break;
	case 2:
		matrizFiltroPasoBajo();
		break;
	case 3:
		matrizFiltroPasoAlto3x3();
		break;
	case 4:
		matrizFiltroPasoAlto5x5();
		break;
	case 5:
		matrizFiltroGaussiano3x3();
		break;
	case 6:
		matrizFiltroGaussiano5x5();
		break;
	case 7:
		matrizFiltroLaplaciano3x3();
		break;
	case 8:
		matrizFiltroLaplaciano5x5();
		break;
	case 9:
		matrizFiltroSobelX();
		break;
	case 10:
		matrizFiltroSobelY();
		break;
	case 11:
		matrizFiltroPrewittX();
		break;
	case 12:
		matrizFiltroPrewittY();
		break;
	default:
		return 0;
	}

	imagenSalida = imagenEntrada;


	//Cálculo de la convolución:
	double sumaConvolucion;

	for (int x = imagenEntrada.FirstRow(); x <= imagenEntrada.LastRow(); x++) {
		for (int y = imagenEntrada.FirstCol(); y <= imagenEntrada.LastCol(); y++) {

			sumaConvolucion = 0;
			
			for (int i = matrizConvolucion.FirstRow(); i <= matrizConvolucion.LastRow(); i++) {
				for (int j = matrizConvolucion.FirstCol(); j <= matrizConvolucion.LastRow(); j++) {
					
					sumaConvolucion += CalcularValorPixel(x, y, i, j, matrizConvolucion);
				}
			}
			imagenSalida(x, y) = sumaConvolucion;
		}
	}

	//Reajuste de valores menores de 0 o mayores de 255 para que estén dentro del rango (0,255):
	ReajusteValores();
	//Reajuste de los valore para no perder informacion cuando existan pixeles negativos


	//Escritura de la imagen de salida:
	char nombreImagenSalida[80] = "";
	strcat(nombreImagenSalida, nombreSinFormato);
	strcat(nombreImagenSalida, nombreFiltro);

	imagenSalida.WriteBMP(nombreImagenSalida);
	printf("\nEl filtro se ha aplicado correctamente. Se ha creado una nueva imagen llamada '%s'.\n", nombreImagenSalida);

	system("Pause");
}


int CalcularValorPixel(int x, int y, int i, int j, C_Matrix matrizConvolucion) {

	// OJO:	No confundir x e y con xConv e yConv, todas actuan recorriendo la imagen de entrada pero x e y
	//		muestran el pixel de destino mientras que xConv e yConv muestran sobre que pixel se está realizando
	//		el calculo con la matriz de convolucion.

	int xConv = (x - (matrizConvolucion.LastRow() / 2)) + i;
	int yConv = (y - (matrizConvolucion.LastRow() / 2)) + j;
	
	//Si el pixel está fuera de la imagen (tratamiento de bordes):
	
	// 1.Esquina superior izquierda:
	if (xConv < imagenEntrada.FirstRow() && yConv < imagenEntrada.FirstCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.FirstRow(), imagenEntrada.FirstCol());

	// 2.Esquina superior derecha:
	else if(xConv < imagenEntrada.FirstRow() && yConv > imagenEntrada.LastCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.FirstRow(), imagenEntrada.LastCol());

	// 3.Esquina inferior izquierda:
	else if (xConv > imagenEntrada.LastRow() && yConv < imagenEntrada.FirstCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.LastRow(), imagenEntrada.FirstCol());

	// 4.Esquina inferior derecha:
	else if (xConv > imagenEntrada.LastRow() && yConv > imagenEntrada.LastCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.LastRow(), imagenEntrada.LastCol());

	// 5.Lado superior:
	else if (xConv < imagenEntrada.FirstRow())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.FirstRow(), yConv);

	// 6.Lado izquierdo:
	else if (yConv < imagenEntrada.FirstCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(xConv, imagenEntrada.FirstCol());

	// 7.Lado inferior:
	else if (xConv > imagenEntrada.LastRow())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(imagenEntrada.LastRow(), yConv);

	// 8.Lado derecho:
	else if (yConv > imagenEntrada.LastCol())
		return (double)matrizConvolucion(i, j) * (double)imagenEntrada(xConv, imagenEntrada.LastCol());

	// 9.Por defecto sin tratamiento de bordes:
	return  (double)matrizConvolucion(i, j) *  (double)imagenEntrada(xConv, yConv);
}

void ReajusteValores() {
	double limiteInferior = imagenSalida.Min();
	double limiteSuperior = imagenSalida.Max();

	for (int x = imagenSalida.FirstRow(); x <= imagenSalida.LastRow(); x++) {
		for (int y = imagenSalida.FirstCol(); y <= imagenSalida.LastCol(); y++) {
			imagenSalida(x, y) =((imagenSalida(x,y)*limiteInferior)/0)+((imagenSalida(x, y)*limiteSuperior)/255) ;
		}
	}
}

void matrizFiltroMedia() {
	C_Matrix kernel(0, 2, 0, 2, 1);
	// ___________
	//|_1_|_1_|_1_|
	//|_1_|_1_|_1_|
	//|_1_|_1_|_1_|

	printf("\n\nHas seleccionado el Filtro Media, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_media.bmp");
}

void matrizFiltroPasoBajo() {
	C_Matrix kernel(0, 2, 0, 2, 0.13);
	// ______________
	//|0.13|0.13|0.13|
	//|0.13|__0_|0.13|
	//|0.13|0.13|0.13|
	kernel(1, 1) = 0;

	printf("\n\nHas seleccionado el Filtro Paso Bajo, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_pasoBajo.bmp");
}

void matrizFiltroPasoAlto3x3() {
	C_Matrix kernel(0, 2, 0, 2, -1);
	// ___________
	//|-1_|-1_|-1_|
	//|-1_|_8_|-1_|
	//|-1_|-1_|-1_|

	kernel(1, 1) = 8;

	printf("\n\nHas seleccionado el Filtro Paso Alto (3x3), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_pasoAlto_3x3.bmp");
}

void matrizFiltroPasoAlto5x5() {
	C_Matrix kernel(0, 4, 0, 4, -1);
	// ___________________
	//|-1_|-1_|-1_|-1_|-1_|
	//|-1_|-1_|-1_|-1_|-1_|
	//|-1_|-1_|25_|-1_|-1_|
	//|-1_|-1_|-1_|-1_|-1_|
	//|-1_|-1_|-1_|-1_|-1_|

	kernel(2, 2) = 25;

	printf("\n\nHas seleccionado el Filtro Paso Alto (5x5), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_pasoAlto_5x5.bmp");
}

void matrizFiltroLaplaciano3x3() {
	C_Matrix kernel(0, 2, 0, 2, 1);
	// ___________
	//|_0_|_1_|_0_|
	//|_1_|-4_|_1_|
	//|_0_|_1_|_0_|

	
	kernel(0, 0) = 0; kernel(0, 2) = 0;
	kernel(1, 1) = -4;
	kernel(2, 0) = 0; kernel(2, 2) = 0;

	printf("\n\nHas seleccionado el Filtro Laplaciano (3x3), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_laplaciano_3x3.bmp");
}

void matrizFiltroLaplaciano5x5() {
	C_Matrix kernel(0, 4, 0, 4, 0);
	// ___________________
	//|_0_|_0_|-1_|_0_|_0_|
	//|_0_|-1_|-2_|-1_|_0_|
	//|-1_|-2_|17_|-2_|-1_|
	//|_0_|-1_|-2_|-1_|_0_|
	//|_0_|_0_|-1_|_0_|_0_|

	kernel(0, 2) = -1;
	kernel(1, 1) = -1;	kernel(1, 2) = -2; 	kernel(1, 3) = -1;
	kernel(2, 0) = -1; 	kernel(2, 1) = -2;	kernel(2, -2) = 17; kernel(2, 3) = -2; kernel(2, 4) = -1;
	kernel(3, 1) = -1;	kernel(3, 2) = -2; 	kernel(3, 3) = -1;
	kernel(4, 2) = -1;

	printf("\n\nHas seleccionado el Filtro Laplaciano (5x5), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_laplaciano_5x5.bmp");
}

void matrizFiltroSobelX() {
	C_Matrix kernel(0, 2, 0, 2, 0);
	// ___________
	//|_1_|_0_|-1_|
	//|_2_|_0_|-2_|
	//|_1_|_0_|-1_|
	kernel(0, 0) = 1; kernel(0, 1) = 0; kernel(0, 2) = -1;
	kernel(1, 0) = 2; kernel(1, 1) = 0; kernel(1, 2) = -2;
	kernel(2, 0) = 1; kernel(1, 1) = 0; kernel(2, 2) = -1;

	printf("\n\nHas seleccionado el Filtro Sobel X, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_sobelX.bmp");
}

void matrizFiltroSobelY() {
	C_Matrix kernel(0, 2, 0, 2, 0);
	// ___________
	//|_1_|_2_|_1_|
	//|_0_|_0_|_0_|
	//|-1_|_2_|-1_|
	kernel(0, 0) = 1; kernel(0, 1) = 2; kernel(0, 2) = 1;
	kernel(1, 0) = 0; kernel(1, 1) = 0; kernel(1, 1) = 0;
	kernel(2, 0) = -1; kernel(2, 1) = -2; kernel(2, 2) = -1;

	printf("\n\nHas seleccionado el Filtro Sobel Y, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_sobelY.bmp");
}

void matrizFiltroGaussiano3x3() {
	C_Matrix kernel(0, 2, 0, 2, 0.13);
	// ________________
	//|0.063|0.13|0.063|
	//|0.13_|0.25|0.13_|
	//|0.063|0.13|0.063|

	kernel(0, 0) = 0.063; kernel(0, 2) = 0.063;
	kernel(1, 1) = 0.25;
	kernel(2, 0) = 0.063; kernel(2, 2) = 0.063;

	printf("\n\nHas seleccionado el Filtro Gaussiano (3x3), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_gaussiano_3x3.bmp");
}

void matrizFiltroGaussiano5x5() {
	C_Matrix kernel(0, 4, 0, 4, 0.015);
	// _______________________________
	//|0.0037|0.015|0.026|0.015|0.0037|
	//|0.015_|0.059|0.095|0.059|0.015_|
	//|0.026_|0.095|0.15_|0.095|0.026_|
	//|0.015_|0.059|0.095|0.059|0.015_|
	//|0.0037|0.015|0.026|0.015|0.0037|

	kernel(0, 0) = 0.0037; kernel(0, 2) = 0.026; kernel(0, 4) = 0.0037;
	kernel(1, 1) = 0.059; kernel(1, 2) = 0.095; kernel(1, 3) = 0.059;
	kernel(2, 0) = 0.026; kernel(2, 1) = 0.095; kernel(2, 2) = 0.15; kernel(2, 3) = 0.095; kernel(2, 4) = 0.026;
	kernel(3, 1) = 0.059; kernel(3, 2) = 0.095; kernel(3, 3) = 0.059;
	kernel(4, 0) = 0.0037; kernel(4, 2) = 0.026; kernel(4, 4) = 0.0037;

	printf("\n\nHas seleccionado el Filtro Gaussiano (5x5), esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_gaussiano_5x5.bmp");
}

void matrizFiltroPrewittY() {
	C_Matrix kernel(0, 2, 0, 2, 0);
	// ___________
	//|_1_|_1_|_1_|
	//|_0_|_0_|_0_|
	//|-1_|-1_|-1_|
	kernel(0, 0) = 1; kernel(0, 1) = 1; kernel(0, 2) = 1;
	kernel(2, 0) = -1; kernel(2, 1) = -1; kernel(2, 2) = -1;

	printf("\n\nHas seleccionado el Filtro Prewitt Y, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_prewittY.bmp");
}

void matrizFiltroPrewittX() {
	C_Matrix kernel(0, 2, 0, 2, 0);
	// ___________
	//|_1_|_0_|-1_|
	//|_1_|_0_|-1_|
	//|_1_|_0_|-1_|
	kernel(0, 0) = 1; kernel(0, 2) = -1;
	kernel(1, 0) = 1; kernel(1, 2) = -1;
	kernel(2, 0) = 1; kernel(2, 2) = -1;

	printf("\n\nHas seleccionado el Filtro Prewitt X, esta es su matriz de convolucion:\n\n");
	matrizConvolucion = kernel;
	matrizConvolucion.Print(matrizConvolucion.ColN(), (int)C_DOUBLE_P_INF);

	strcat(nombreFiltro, "_prewittX.bmp");
}