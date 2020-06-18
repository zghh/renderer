#include "areaLight.h"

void AreaLight::Init(Point &a, Point &b, Point &c, Point &color)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->dx = sub(b, a);
    this->dy = sub(c, a);
    this->n = mul(this->dx, this->dy);
    this->area = sqrt(dot(this->n, this->n)) / 2;
    normalization(this->n);
    this->color = color;

    this->sample = 1;
    this->randomEngine = default_random_engine(1);
}

Point AreaLight::GetLight(const Point &p, const Point &n, const Point &d, const Point &kd, const Point &ks, const double ns, KDTree *kdTree, int num, int nSample)
{
    Point result = Point{0, 0, 0};
    uniform_real_distribution<double> u(0, 1);
    if (nSample == 0)
        nSample = this->sample;
    for (int i = 0; i < nSample; i++)
    {
        double x = u(this->randomEngine), y = u(this->randomEngine);
        Point o = add(add(mul(this->dx, x), mul(this->dy, y * (1 - x))), this->a);
        Point lightDirection = sub(o, p);
        if (le(dot(n, lightDirection), 0) || le(dot(n, d), 0))
            continue;
        Point tp, tn;
        Texture tTexture;
        Material *tMaterial;
        if (!kdTree->IsHit(p, lightDirection, tp, tn, tTexture, tMaterial, num, 1.0))
        {
            double rr = dot(lightDirection, lightDirection);
            normalization(lightDirection);
            double cosIn = max(dot(n, lightDirection), 0.0);
            double cosOut = max(dot(Point{-lightDirection.x, -lightDirection.y, -lightDirection.z}, this->n), 0.0);
            double factor = cosIn / rr;
            double intensity = factor * this->area;

            if (!eq(kd.x, 0) && !eq(kd.y, 0) && !eq(kd.z, 0))
            {
                double diffuseFactor = max(0.0, dot(n, lightDirection));
                Point diffuse = mul(kd, diffuseFactor * intensity);
                result = add(result, diffuse);
            }

            if (!eq(ks.x, 0) && !eq(ks.y, 0) && !eq(ks.z, 0))
            {
                Point r = sub(mul(n, dot(lightDirection, n) * 2), lightDirection);
                normalization(r);
                double reflectiveFactor = max(0.0, dot(r, d));
                double specularFactor = pow(reflectiveFactor, ns);
                Point specular = mul(ks, specularFactor * ns * intensity);
                result = add(result, specular);
            }
        }
    }
    result = div(dotMul(result, color), nSample);
    limit(result, 1.0);
    return result;
}