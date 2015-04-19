#include <iostream>
#include <windows.h>
#include <cmath>
#include <gl/gl.h>
#include <gl/glut.h>

// Funkcaja okre�laj�ca, co ma by� rysowane
// (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)

float ZAKRES = 7.0f;
float MIN_X = -7.0F;
float MAX_X = 7.0f;
float MIN_Y = -7.0f;
float MAX_Y = 7.0f;
float ZOOM_PLUS = 0.3f;
float ZOOM_MINUS = 0.3f;

int horiz;
int vert;

void ChangeSize(GLsizei horizontal, GLsizei vertical);

float norma(double x, double y)
{
	return pow(pow(x, 2.0) + pow(y, 2.0), 0.5);
}

int zbieznosc(double px, double py)
{
	double zx = px, zy = py;
	double tempx, tempy;

	for (int a = 0; a<33; ++a)
	{
		if (norma(zx, py)<20.0)
		{
			tempx = pow(zx, 2.0) - pow(zy, 2.0) + px;
			tempy = 2 * zx*zy + py;
			zx = tempx;
			zy = tempy;
		}
		else
			return a;
	}
	return 16;  // 16 zamiast 32, poniewa� daje �adny efekt
}

void RenderScene(void)   // wspolrzedne przy rysowaniu rozpatrujemy w zakresie x(MIN_X,MAX_X) y(MIN_Y,MAX_Y)
{
	double i = MIN_X, j = MIN_Y;
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	//glColor3f(0.0f, 1.0f, 0.0f);
	// Ustawienie aktualnego koloru rysowania na zielony
	float odcien;
	double  krok;
	if (horiz>vert)
		krok = (MAX_X - MIN_X) / horiz;
	else
		krok = (MAX_Y - MIN_Y) / vert;

	glPointSize(1);
	while (i <= MAX_X)
	{
		j = MIN_Y;
		while (j <= MAX_Y)
		{
			odcien = 8 * zbieznosc(i, j) / 256.0;

			glColor3f(odcien, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2f(i, j);
			glEnd();
			j += krok;
		}
		i += krok;
	}


	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania
}

/*void glutMouseFunc(mouseHandlerLeft, GLUT_LEFT_BUTTON, int state, int x, int y)
{

}*/

void mouseHandler(int button, int state, int x, int y)   // LPM - przybli�enie , PPM - oddalenie 
{
	if (state != GLUT_UP) return;
	if (button == GLUT_LEFT_BUTTON)
	{
		float zmniejszenie_X = (MAX_X - MIN_X)*ZOOM_PLUS;
		float zmniejszenie_Y = (MAX_Y - MIN_Y)*ZOOM_PLUS;

		MIN_X += zmniejszenie_X*x / horiz;
		MAX_X -= zmniejszenie_X*(horiz - x) / horiz;

		MIN_Y += zmniejszenie_Y*(vert - y) / vert;
		MAX_Y -= zmniejszenie_Y* y / vert;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		float zwiekszenie_X = (MAX_X - MIN_X)*ZOOM_MINUS;
		float zwiekszenie_Y = (MAX_Y - MIN_Y)*ZOOM_MINUS;

		MIN_X -= zwiekszenie_X*x / horiz;
		MAX_X += zwiekszenie_X*(horiz - x) / horiz;

		MIN_Y -= zwiekszenie_Y*(vert - y) / vert;
		MAX_Y += zwiekszenie_Y* y / vert;
	}
	ChangeSize(horiz, vert);
	RenderScene();
}

// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor okna wn�trza okna - ustawiono na szary
}

// Funkcja s�u��ca do kontroli zachowania proporcji rysowanych obiekt�w
// niezale�nie od rozmiar�w okna graficznego
void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s�
// przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna
{
	horiz = horizontal;
	vert = vertical;

	GLfloat AspectRatio;

	// Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna
	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna urz�dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Okre�lenie uk�adu wsp�rz�dnych obserwatora

	glLoadIdentity();
	// Okre�lenie przestrzeni ograniczaj�cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// okre�lenie okna obserwatora.
	// Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
	// Do okre�lenia okna obserwatora s�u�y funkcja glOrtho(...)

	if (horizontal >= vertical)
		glOrtho(MIN_X, MAX_X, MIN_Y / AspectRatio, MAX_Y / AspectRatio, 1.0, -1.0);
	else
		glOrtho(MIN_X*AspectRatio, MAX_X*AspectRatio, MIN_Y, MAX_Y, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okre�lenie uk�adu wsp�rz�dnych    

	glLoadIdentity();
}

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
void main(void)
{
	std::cout << "Sterowanie:\n\tLPM - zblizenie\n\tPPM - oddalenie\n";

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wy�wietlania
	// GLUT_SINGLE - pojedynczy bufor wy�wietlania
	// GLUT_RGBA - model kolor�w RGB

	glutCreateWindow("Zbior Mandelbrota w openGL");
	// Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
	// Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy
	// trzeba b�dzie przerysowa� okno

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie 
	// inicjalizacje konieczneprzed przyst�pieniem do renderowania

	glutMouseFunc(mouseHandler);
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}