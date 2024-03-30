
#include "commons.h"

std::vector<std::unique_ptr<Shape>> g_shapeList;

// 화면 해상도
int g_width = 512;
int g_height = 512;

// 설정
float g_l = -0.1;
float g_r = 0.1;
float g_b = -0.1;
float g_t = 0.1;
float g_d = 0.1;

void render() {

    std::vector<Vec3> pixels(g_width * g_height);

    // 카메라
    Vec3 eye(0, 0, 0);

    // 각 픽셀에 대해 체크
    for (int y= 0; y < g_height; y++) 
    {
        for (int x = 0; x < g_width; x++) {

            float u = g_l + (g_r - g_l) * (x + 0.5) / g_width;
            float v = g_b + (g_t - g_b) * (y + 0.5) / g_height;

            Vec3 rayDirection = Vec3(u, v, -g_d).normalize();

            float min = -1;

            for (const auto& shape : g_shapeList) 
            {
                HitInfo hitInfo = shape->intersect(eye, rayDirection);
                if (hitInfo.hit && ( min < 0 || hitInfo.distance < min ) )
                {
                    min = hitInfo.distance;
                }
            }

            if ( min > 0 ) 
            {
                pixels[y * g_width + x] = Vec3(1, 1, 1);
            }
            else
            {
                pixels[y * g_width + x] = Vec3(0, 0, 0);
            }
        }
    }

    // 그리기
    glDrawPixels(g_width, g_height, GL_RGB, GL_FLOAT, pixels.data());
}


void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    render();

    glutSwapBuffers();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_width, g_height);
    glutInitWindowPosition(100, 100);

    g_shapeList.push_back(std::make_unique<Plane>(Vec3(0, -2, 0), Vec3(0, 1, 0)));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(-4, 0, -7), 1));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(0, 0, -7), 2));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(4, 0, -7), 1));

    glutCreateWindow("HW1 Ray Tracing");

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}