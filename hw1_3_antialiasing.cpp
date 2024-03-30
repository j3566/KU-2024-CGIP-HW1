
#include "commons.h"

std::vector<std::unique_ptr<Shape>> g_shapeList;

// 화면 해상도
int g_width = 512;
int g_height = 512;

float g_l = -0.1;
float g_r = 0.1;
float g_b = -0.1;
float g_t = 0.1;
float g_d = 0.1;

float g_gamma = 2.2;

Vec3 g_lightPosition = Vec3(-4, 4, -3);
Vec3 g_lightIntensity = Vec3(1, 1, 1);
Vec3 g_ambientIntensity = Vec3(0.5, 0.5, 0.5);

int g_N = 64;

Vec3 computePixel(Shape* hitShape, HitInfo& hitInfo, Vec3& rayDirection)
{
    Vec3 hitPoint = hitInfo.point;
    Vec3 hitShapeNormal = hitShape->getNormalVector(hitPoint);
    Vec3 viewDirection = rayDirection * -1;
    Vec3 lightDirection = g_lightPosition - hitPoint;
    float lightDistance = lightDirection.norm();
    Vec3 normalizeLightDirection = lightDirection.normalize();

    Vec3 shadowCheckOrigin = hitPoint + hitShapeNormal * 1e-5;

    float shadowMin = -1;

    for (const auto& shape : g_shapeList)
    {
        HitInfo shapeHitInfo = shape->intersect(shadowCheckOrigin, normalizeLightDirection);
        if (shapeHitInfo.hit && (shadowMin < 0 || shapeHitInfo.distance < shadowMin))
        {
            shadowMin = shapeHitInfo.distance;
        }
    }

    Vec3 ambient = hitShape->ka * g_ambientIntensity;

    Vec3 pixel;

    if (shadowMin > 0 && shadowMin < lightDistance )
    {
        pixel = ambient;
    }
    else
    {
        float diffuseIntensity = std::max(0.0f, normalizeLightDirection.dot(hitShapeNormal));
        Vec3 diffuse = hitShape->kd * g_lightIntensity * diffuseIntensity;

        Vec3 reflectDirection = 2 * hitShapeNormal * normalizeLightDirection.dot(hitShapeNormal) - normalizeLightDirection;
        float specularIntensity = std::pow(std::max(0.0f, reflectDirection.dot(viewDirection)), hitShape->specularPower);
        Vec3 specular = hitShape->ks * g_lightIntensity * specularIntensity;

        pixel = ambient + diffuse +specular;
    }

    return pixel.pow(1 / g_gamma);
}

void render() {

    std::vector<Vec3> pixels(g_width * g_height);

    // 카메라
    Vec3 eye(0, 0, 0);

    // 각 픽셀에 대해 체크
    for (int y= 0; y < g_height; y++) 
    {
        for (int x = 0; x < g_width; x++) {

            Vec3 sumColor = Vec3(0, 0, 0);

            for ( int i = 0, l = g_N; i < l; i++ )
            {
                float u = g_l + (g_r - g_l) * (x + (float)rand() / RAND_MAX - 0.5) / g_width;
                float v = g_b + (g_t - g_b) * (y + (float)rand() / RAND_MAX - 0.5) / g_height;

                Vec3 rayDirection = Vec3(u, v, -g_d).normalize();

                Shape* hitShape = NULL;
                HitInfo hitInfo = {};
                float min = -1;

                for (const auto& shape : g_shapeList)
                {
                    HitInfo shapeHitInfo = shape->intersect(eye, rayDirection);
                    if (shapeHitInfo.hit && (min < 0 || shapeHitInfo.distance < min))
                    {
                        min = shapeHitInfo.distance;
                        hitInfo = shapeHitInfo;
                        hitShape = shape.get();
                    }
                }

                if (hitInfo.hit)
                {
                    sumColor = sumColor + computePixel(hitShape, hitInfo, rayDirection);
                }
                else
                {
                    sumColor = sumColor + Vec3(0, 0, 0);
                }
            }

            pixels[y * g_width + x] = sumColor / g_N;
        }
    }
   
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

    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_width, g_height);
    glutInitWindowPosition(100, 100);

    g_shapeList.push_back(std::make_unique<Plane>(
        Vec3(0, -2, 0), Vec3(0, 1, 0),
        Vec3(0.2, 0.2, 0.2), Vec3(1, 1, 1), Vec3(0, 0, 0), 0
    ));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(-4, 0, -7), 1,
        Vec3(0.2, 0, 0), Vec3(1, 0, 0), Vec3(0, 0, 0), 0
    ));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(0, 0, -7), 2,
        Vec3(0, 0.2, 0), Vec3(0, 0.5, 0), Vec3(0.5, 0.5, 0.5), 32
    ));
    g_shapeList.push_back(std::make_unique<Sphere>(Vec3(4, 0, -7), 1,
        Vec3(0, 0, 0.2), Vec3(0, 0, 1), Vec3(0, 0, 0), 0
    ));

    glutCreateWindow("HW1 Antialiasing");

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}