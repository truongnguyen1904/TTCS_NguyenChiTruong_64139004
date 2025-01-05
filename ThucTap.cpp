#include <conio.h>
#include <graphics.h>

#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

int startX, startY, endX, endY;
bool isDrawing = false;

int shape = 0;               // luu hinh ve duoc chon
bool shapeSelected = false;  // ban dau chua chon hình
int currentColor = WHITE;    // mau ban dau la trang

struct Shape {
  int type;                        // loai hinh
  int x1, y1, x2, y2;              // toa do dung cho duong thang, hinh vuong
  int radius;                      // ban kinh dung cho hinh ngoi sao, tron
  int color;                       // mau
  vector<pair<int, int> > points;  // luu cac diem cua duong tu do
};

vector<Shape> shapes;

void drawLineBresenham(int x0, int y0, int x1, int y1, int color) {
  int dx = abs(x1 - x0), dy = abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;

  if (dx >= dy) {
    // Truong hop |m| <= 1
    int err = 2 * dy - dx;
    for (int i = 0; i <= dx; ++i) {
      putpixel(x0, y0, color);
      if (err > 0) {
        y0 += sy;
        err -= 2 * dx;
      }
      x0 += sx;
      err += 2 * dy;
    }
  } else {
    // Truong hop m > 1
    int err = 2 * dx - dy;
    for (int i = 0; i <= dy; ++i) {
      putpixel(x0, y0, color);
      if (err > 0) {
        x0 += sx;
        err -= 2 * dy;
      }
      y0 += sy;
      err += 2 * dx;
    }
  }
}

// Ham ve duong tu do noi cac diem lien tiep
void drawFreeformLine(const vector<pair<int, int> >& points, int color) {
  for (size_t i = 0; i < points.size() - 1; ++i) {
    int x0 = points[i].first;
    int y0 = points[i].second;
    int x1 = points[i + 1].first;
    int y1 = points[i + 1].second;
    drawLineBresenham(x0, y0, x1, y1, color);
  }
}



void drawRectangleBresenham(int x1, int y1, int x2, int y2, int color) {
  drawLineBresenham(x1, y1, x2, y1, color);
  drawLineBresenham(x2, y1, x2, y2, color);
  drawLineBresenham(x2, y2, x1, y2, color);
  drawLineBresenham(x1, y2, x1, y1, color);
}

void drawCircleBresenham(int x_center, int y_center, int r, int color) {
  int x = 0, y = r, d = 3 - 2 * r;
  while (x <= y) {
    putpixel(x_center + x, y_center + y, color);
    putpixel(x_center - x, y_center + y, color);
    putpixel(x_center + x, y_center - y, color);
    putpixel(x_center - x, y_center - y, color);
    putpixel(x_center + y, y_center + x, color);
    putpixel(x_center - y, y_center + x, color);
    putpixel(x_center + y, y_center - x, color);
    putpixel(x_center - y, y_center - x, color);
    if (d < 0)
      d += 4 * x + 6;
    else {
      d += 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void drawStarBresenham(int x_center, int y_center, int radius, int color) {
  const int numPoints = 5;
  double angle = M_PI / numPoints;
  int x[numPoints], y[numPoints];
  for (int i = 0; i < numPoints; i++) {
    x[i] = x_center + radius * cos(i * 2 * angle - M_PI / 2);
    y[i] = y_center + radius * sin(i * 2 * angle - M_PI / 2);
  }
  for (int i = 0; i < numPoints; i++) {
    int next = (i + 2) % numPoints;
    drawLineBresenham(x[i], y[i], x[next], y[next], color);
  }
}
// Hàm ve duong cong parabol don gian
void drawCurve(int startX, int startY, int endX, int endY, int numPoints, int color) {
  vector<pair<int, int> > points;

// Tao cac diem doc theo mot duong cong parabol
  for (int i = 0; i < numPoints; ++i) {
    int x = startX + (endX - startX) * i / (numPoints - 1);        // Phân ph?i d?u các di?m theo chi?u ngang
    int y = startY + (x - startX) * (x - endX) / (numPoints * 2);  // Công th?c t?o du?ng cong parabol
    points.push_back({x, y});
  }

  // Ve duong cong
  drawFreeformLine(points, color);
}
bool isPointInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3) {
  // Tinh S tam giac
  int areaOrig = abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));

  // Tinh S 3 tam giac nho
  int area1 = abs(px * (y2 - y3) + x2 * (y3 - py) + x3 * (py - y2));
  int area2 = abs(x1 * (py - y3) + px * (y3 - y1) + x3 * (y1 - py));
  int area3 = abs(x1 * (y2 - py) + x2 * (py - y1) + px * (y1 - y2));

  // Kiem tra dien tich cua 3 tam giac nho co bang tam giac lon
  return (area1 + area2 + area3 == areaOrig);
}
bool isPointInStar(int px, int py, int cx, int cy, int radius_outer, int radius_inner) {
  const int numPoints = 5;  // So dinh cua ngôi sao
  double angle = M_PI / numPoints;
  int x[2 * numPoints], y[2 * numPoints];

  // Tinh toa do các dinh cua ngoi sao
  for (int i = 0; i < 2 * numPoints; i++) {
    double r = (i % 2 == 0) ? radius_outer : radius_inner;
    x[i] = cx + r * cos(i * angle - M_PI / 2);
    y[i] = cy + r * sin(i * angle - M_PI / 2);
  }

  // Kiem tra tung tam giac tao boi tam và các canh cua ngoi sao
  for (int i = 0; i < 2 * numPoints; i++) {
    int next = (i + 1) % (2 * numPoints);
    if (isPointInTriangle(px, py, cx, cy, x[i], y[i], x[next], y[next])) {
      return true;
    }
  }

  return false;  // Neu không nam trong tam giác nào, tra ve false
}

double distanceFromPointToLine(int px, int py, int x1, int y1, int x2, int y2) {
  // Khoang cach tu diem den duong thang
  double num = abs((y2 - y1) * px - (x2 - x1) * py + x2 * y1 - y2 * x1);
  double den = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
  return num / den;
}
double distanceBetweenPoints(int x1, int y1, int x2, int y2) { return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)); }
void drawInterface() {
  setcolor(WHITE);

  int drawingAreaHeight = getmaxy();

  // Thanh công cu
  rectangle(0, 0, getmaxx(), 60);
  setfillstyle(SOLID_FILL, LIGHTGRAY);
  floodfill(20, 20, WHITE);

  // Cac nut bam
  // Nut ve duong thang
  rectangle(10, 10, 100, 50);
  drawLineBresenham(20, 30, 90, 30, WHITE);

  // Nut ve hình chu nhat
  rectangle(110, 10, 200, 50);
  drawRectangleBresenham(120, 20, 190, 40, WHITE);

  // Nut ve hình tròn
  rectangle(210, 10, 300, 50);
  drawCircleBresenham(255, 30, 15, WHITE);

  // Nut ve hình tam giác
  rectangle(310, 10, 400, 50);
  int x1 = 355, y1 = 15, x2 = 325, y2 = 45, x3 = 385, y3 = 45;
  drawLineBresenham(x1, y1, x2, y2, WHITE);
  drawLineBresenham(x2, y2, x3, y3, WHITE);
  drawLineBresenham(x3, y3, x1, y1, WHITE);

  // Nut ve ngôi sao
  rectangle(410, 10, 500, 50);
  const int numPoints = 5;
  int cx = 455, cy = 30, radius = 20;
  drawStarBresenham(cx, cy, radius, WHITE);

  // Nut ve tu do
  rectangle(510, 10, 600, 50);
  drawCurve(510, 30, 600, 130, 100, WHITE);  // 100 di?m, màu tr?ng

  // Nut cuc tay
  rectangle(610, 10, 700, 50);
  outtextxy(640, 20, "Erase");

  // Cac o mau
  // Dat cac o mau 
  setfillstyle(SOLID_FILL, RED);
  rectangle(710, 10, 750, 50);
  floodfill(720, 20, WHITE);

  setfillstyle(SOLID_FILL, GREEN);
  rectangle(760, 10, 800, 50);
  floodfill(770, 20, WHITE);

  setfillstyle(SOLID_FILL, BLUE);
  rectangle(810, 10, 850, 50);
  floodfill(820, 20, WHITE);

  setfillstyle(SOLID_FILL, MAGENTA);
  rectangle(860, 10, 900, 50);
  floodfill(870, 20, WHITE);

  setfillstyle(SOLID_FILL, YELLOW);
  rectangle(910, 10, 950, 50);
  floodfill(920, 20, WHITE);

  setfillstyle(SOLID_FILL, CYAN);
  rectangle(960, 10, 1000, 50);
  floodfill(970, 20, WHITE);

  setfillstyle(SOLID_FILL, WHITE);
  rectangle(1010, 10, 1050, 50);
  floodfill(1020, 20, WHITE);

  // Vung ve
  rectangle(0, 60, getmaxx(), getmaxy());
}

void handleMouseClick(int x, int y) {
  if (y >= 10 && y <= 50) {
// Kiem tra cac nut hinh
    if (x >= 10 && x <= 100)
      shape = 1;  // Chon duong thang

    else if (x >= 110 && x <= 200)
      shape = 2;  // Chon hinh vuong
    else if (x >= 210 && x <= 300)
      shape = 3;  // Chon hinh tron
    else if (x >= 310 && x <= 400)
      shape = 4;  // Chon tam giác
    else if (x >= 410 && x <= 500)
      shape = 5;  // Chon ngôi sao
    else if (x >= 510 && x <= 600)
      shape = 6;  // Chon ve tu do

    // Chon cuc tay
    else if (x >= 610 && x <= 700) {
      shape = 0;             // Bo chon hinh
      currentColor = WHITE;  
    }

    // Ch?n màu
    else if (x >= 710 && x <= 750)
      currentColor = RED;  // Mau do
    else if (x >= 760 && x <= 800)
      currentColor = GREEN;  // Mau xanh la
    else if (x >= 810 && x <= 850)
      currentColor = BLUE;  // Mau xanh duong
    else if (x >= 860 && x <= 900)
      currentColor = MAGENTA;  // Mau hong
    else if (x >= 910 && x <= 950)
      currentColor = YELLOW;  // Mau vang
    else if (x >= 960 && x <= 1000)
      currentColor = CYAN;  // Mau xanh nhat
    else if (x >= 1010 && x <= 1050)
      currentColor = WHITE;
    if (shape == 6 && isDrawing) {
      // Luu các diem ve tu do
      Shape freeDrawShape;
      freeDrawShape.type = 6;
      freeDrawShape.points.push_back({x, y});
      freeDrawShape.color = currentColor;
      shapes.push_back(freeDrawShape);
    }

    shapeSelected = true;  
  }
}

void drawShape() {
  if (startY < 60) {
    return;  // neu chuot nam trong vung giao dien => khong ve
  }

  if (!shapeSelected) return;  // neu hinh chua duoc chon => khong ve

  // ve giao dien
  for (size_t i = 0; i < shapes.size(); ++i) {
    const Shape& s = shapes[i];
    if (s.type == 6) {  // Ve tu do
      drawFreeformLine(s.points, s.color);
    }
  }

  // ve lai cac hinh da luu

  for (int i = 0; i < shapes.size(); i++) {
    Shape s = shapes[i];
    // kiem tra hinh co nam trong vung ve khong
    if (s.x1 < 10 || s.y1 < 60 || s.x2 > getmaxx() || s.y2 > getmaxy()) {
      continue;  // bo qua hinh neu nam ngoai vung ve
    }

    // ve hinh dua tren loai hinh

    switch (s.type) {
      case 1:
        drawLineBresenham(s.x1, s.y1, s.x2, s.y2, s.color);
        break;
      case 2:
        drawRectangleBresenham(s.x1, s.y1, s.x2, s.y2, s.color);
        break;
      case 3:
        drawCircleBresenham(s.x1, s.y1, s.radius, s.color);
        break;
      case 4: {
        int size = abs(s.x2 - s.x1);
        int height = static_cast<int>(size * sqrt(3) / 2);
        int x1 = s.x1, y1 = s.y1 - height;
        int x2 = s.x1 - size / 2, y2 = s.y1 + height / 2;
        int x3 = s.x1 + size / 2, y3 = s.y1 + height / 2;
        drawLineBresenham(x1, y1, x2, y2, s.color);
        drawLineBresenham(x2, y2, x3, y3, s.color);
        drawLineBresenham(x3, y3, x1, y1, s.color);
        break;
      }
      case 5:
        drawStarBresenham(s.x1, s.y1, s.radius, s.color);
        break;
    }
  }  
  if (startY > 60 && endY >= 60 && endX >= 10 && endX <= getmaxx() - 10) {
    if (shape == 0) {
      for (int i = 0; i < shapes.size(); i++) {
        Shape s = shapes[i];
        // Line
        if (s.type == 1) {  // neu la duong thang
          double dist = distanceFromPointToLine(endX, endY, s.x1, s.y1, s.x2, s.y2);
          if (dist < 5) {                      // neu khoang cach nho hon 5 pixel => xoa duong thang
            shapes.erase(shapes.begin() + i);  // xoa hinh
            cleardevice();
            drawInterface();
            drawShape();
            break;
          }

        } else if (s.type == 2) {  // Rectangle
          if (endX >= s.x1 && endX <= s.x2 && endY >= s.y1 && endY <= s.y2) {
            shapes.erase(shapes.begin() + i);  // xoa hinh
            cleardevice();
            drawInterface();
            drawShape();
            break;
          }
        } else if (s.type == 3) {  // Circle
          int distance = sqrt(pow(s.x1 - endX, 2) + pow(s.y1 - endY, 2));
          if (distance <= s.radius) {
            shapes.erase(shapes.begin() + i);
            cleardevice();
            drawInterface();
            drawShape();  // xoa hinh
            break;
          }
        } else if (s.type == 4) {  // Triangle
          int size = abs(s.x2 - s.x1);
          int height = static_cast<int>(size * sqrt(3) / 2);
          int x1 = s.x1, y1 = s.y1 - height;
          int x2 = s.x1 - size / 2, y2 = s.y1 + height / 2;
          int x3 = s.x1 + size / 2, y3 = s.y1 + height / 2;

          // kiem tra diem click chuot co nam trong tam giac khong
          if (isPointInTriangle(endX, endY, x1, y1, x2, y2, x3, y3)) {
            shapes.erase(shapes.begin() + i);  // xoa tam giac
            cleardevice();
            drawInterface();
            drawShape();
            break;
          }
        } else if (s.type == 5) {  // Star
          if (isPointInStar(endX, endY, s.x1, s.y1, s.radius, s.radius / 2.5)) {
            shapes.erase(shapes.begin() + i);  // xoa hinh ngoi sao
            cleardevice();
            drawInterface();
            drawShape();
            break;
          }
        }

        else if (s.type == 6) {  // chon duong tu do
          for (size_t j = 0; j < s.points.size(); j++) {
            int pointX = s.points[j].first;   // lay toa do x
            int pointY = s.points[j].second;  // lay toa do y

// Tinh khoang cach tu diem chuot den diem trong duong tu do
            int distance = sqrt(pow(pointX - endX, 2) + pow(pointY - endY, 2));

// Neu khoang cach nho hon 5 pixel, xoa toan bo duong tu do
            if (distance < 5) {
              shapes.erase(shapes.begin() + i);  // xóa toàn bo duong tu do
              cleardevice();
              drawInterface();
              drawShape();
              break;
            }
          }
        }
      }

    }
    // neu la duong tu do

    else {
      if (shape != 6) {
        cleardevice();  
        drawInterface();
        for (size_t i = 0; i < shapes.size(); ++i) {
          const Shape& s = shapes[i];
          // Ve lai cac duong tu do
          if (s.type == 6) {  // Ðuong tu do
            drawFreeformLine(s.points, s.color);
          }
        }
      } 
      switch (shape) {
        case 1:
          drawLineBresenham(startX, startY, endX, endY, currentColor);
          break;
        case 2:
          drawRectangleBresenham(startX, startY, endX, endY, currentColor);
          break;
        case 3:
          drawCircleBresenham(startX, startY, sqrt(pow(endX - startX, 2) + pow(endY - startY, 2)), currentColor);
          break;
        case 4: {
          int size = abs(endX - startX);
          int height = static_cast<int>(size * sqrt(3) / 2);
          int x1 = startX, y1 = startY - height;
          int x2 = startX - size / 2, y2 = startY + height / 2;
          int x3 = startX + size / 2, y3 = startY + height / 2;
          drawLineBresenham(x1, y1, x2, y2, currentColor);
          drawLineBresenham(x2, y2, x3, y3, currentColor);
          drawLineBresenham(x3, y3, x1, y1, currentColor);
          break;
        }
        case 5: {
          int radius = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2));
          drawStarBresenham(startX, startY, radius, currentColor);
          break;
        }
      }
    }
  }
  for (int i = 0; i < shapes.size(); i++) {
    Shape s = shapes[i];
    // kiem tra hinh co nam trong vung ve khong
    if (s.x1 < 10 || s.y1 < 60 || s.x2 > getmaxx() || s.y2 > getmaxy()) {
      continue;  // bo qua hinh neu nam ngoai vung ve
    }

    // ve hinh dua tren loai hinh

    switch (s.type) {
      case 1:
        drawLineBresenham(s.x1, s.y1, s.x2, s.y2, s.color);
        break;
      case 2:
        drawRectangleBresenham(s.x1, s.y1, s.x2, s.y2, s.color);
        break;
      case 3:
        drawCircleBresenham(s.x1, s.y1, s.radius, s.color);
        break;
      case 4: {
        int size = abs(s.x2 - s.x1);
        int height = static_cast<int>(size * sqrt(3) / 2);
        int x1 = s.x1, y1 = s.y1 - height;
        int x2 = s.x1 - size / 2, y2 = s.y1 + height / 2;
        int x3 = s.x1 + size / 2, y3 = s.y1 + height / 2;
        drawLineBresenham(x1, y1, x2, y2, s.color);
        drawLineBresenham(x2, y2, x3, y3, s.color);
        drawLineBresenham(x3, y3, x1, y1, s.color);
        break;
      }
      case 5:
        drawStarBresenham(s.x1, s.y1, s.radius, s.color);
        break;
    }
  }
// Neu dang ve duong tu do, them cac diem vao danh sach diem cua duong tu do
}
int main() {
  int gd = DETECT, gm;
  initgraph(&gd, &gm, "");

  drawInterface();  // ve giao dien

  while (true) {
    if (ismouseclick(WM_LBUTTONDOWN)) {
      if (shapeSelected == false) {
        cleardevice();
        drawInterface();
      }
      int x, y;
      getmouseclick(WM_LBUTTONDOWN, x, y);
      handleMouseClick(x, y);
      startX = x;
      startY = y;
      isDrawing = true;
      clearmouseclick(WM_LBUTTONDOWN);
      if (shape == 6) {
        Shape newShape;
        newShape.type = shape;
        newShape.color = currentColor;
        newShape.points.clear();                      
        newShape.points.push_back({startX, startY});  
        shapes.push_back(newShape);
      }
    }

  

    if (ismouseclick(WM_MOUSEMOVE) && isDrawing && shapeSelected) {
      int x = mousex();
      int y = mousey();

      if (shape == 6) {
        if (!shapes.empty() && shapes.back().type == 6) {
          shapes.back().points.push_back(make_pair(x, y));
        }
      }
      endX = x;
      endY = y;
      drawShape();

      clearmouseclick(WM_MOUSEMOVE);
    }

    if (ismouseclick(WM_LBUTTONUP)) {
      drawShape();
      int x, y;
      getmouseclick(WM_LBUTTONUP, x, y);
      endX = x;
      endY = y;
      isDrawing = false;

      // luu hinh vao danh sach
      if (shape == 5) {  // ngoi sao
        int radius = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2));
        shapes.push_back({shape, startX, startY, 0, 0, radius, currentColor});
      } else if (shape == 3) {  // hinh tron
        int radius = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2));
        shapes.push_back({shape, startX, startY, 0, 0, radius, currentColor});
      } else if (shape == 1 || shape == 2 || shape == 4) {
        shapes.push_back({shape, startX, startY, endX, endY, 0, currentColor});
      } else if (shape == 6) {
       // Duong tu do
// Khi the chuot, chi can them diem cuoi

        if (!shapes.empty() && shapes.back().type == 6) {
          shapes.back().points.push_back({endX, endY});
        }
      }

      // ve lai cac hinh
      drawShape();
      clearmouseclick(WM_LBUTTONUP);
    }
// Kiem tra nhan chuot de xoa duong tu do neu can

    // kiem tra khi thoat
    if (kbhit()) {
      char key = getch();
      if (key == 27) {  // = nut ESC
        break;          // Thoát chuong trình
      }
    }
  }

  closegraph();  // dong cua so do hoa
  return 0;
}

