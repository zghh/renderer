#include "parser.h"

Parser::Parser(string fileName)
{
    int index = fileName.find_last_of('.');
    this->mtlFileName = fileName.substr(0, index + 1) + "mtl";
    this->informationFileName = fileName.substr(0, index + 1) + "txt";
    index = fileName.find_last_of('/');
    this->folderName = fileName.substr(0, index + 1);
    this->objFileName = fileName;
    this->hasLoaded = false;
    this->nPoint = 0;
    this->nTexture = 0;
    this->nNormalDirection = 0;
    this->nFace = 0;
    this->nMaterial = 0;
    this->hasCameraInformation = false;
}

Parser::~Parser()
{
    if (this->hasLoaded)
    {
        delete[] this->points;
        delete[] this->textures;
        delete[] this->normalDirections;
        delete[] this->faces;
        delete[] this->materials;
    }
}

Face *Parser::GetFaces()
{
    return this->faces;
}

Point Parser::GetPoint(int index)
{
    return this->points[index];
}

Point Parser::GetNormalDirection(int index)
{
    return this->normalDirections[index];
}

Texture Parser::GetTexture(int index)
{
    return this->textures[index];
}

int Parser::GetNFace()
{
    return this->nFace;
}

int Parser::GetNPoint()
{
    return this->nPoint;
}

bool Parser::HasCameraInformation()
{
    return this->hasCameraInformation;
}

Point Parser::GetCameraX()
{
    return this->cameraX;
}

Point Parser::GetCameraY()
{
    return this->cameraY;
}

Point Parser::GetCameraZ()
{
    return this->cameraZ;
}

Point Parser::GetCameraO()
{
    return this->cameraO;
}

double Parser::GetCameraPixelWidth()
{
    return this->cameraPixelWidth;
}

double Parser::GetCameraF()
{
    return this->cameraF;
}

bool Parser::GetCameraProjectionState()
{
    return this->cameraProjectionState;
}

void Parser::Load()
{
    LoadCount();
    LoadData();
    this->hasLoaded = true;
}

void Parser::LoadCount()
{
    char ch;
    this->materialMap.clear();
#ifdef MAP_TEXTURE
    this->textureMap.clear();
#endif

    freopen(this->mtlFileName.c_str(), "r", stdin);
    while (scanf("%c", &ch) != EOF)
    {
        if (ch == 'n')
        {
            scanf("%s", this->tempString);
            scanf("%s", this->tempString);
            materialMap[string(this->tempString)] = this->nMaterial;
            this->nMaterial++;
        }
        else if (ch == '\n' || ch == ' ' || ch == '\t')
        {
            continue;
        }
        LoadLine();
    }

    freopen(this->objFileName.c_str(), "r", stdin);
    while (scanf("%c", &ch) != EOF)
    {
        if (ch == 'v')
        {
            scanf("%c", &ch);
            if (ch == ' ')
                this->nPoint++;
            else if (ch == 't')
                this->nTexture++;
            else if (ch == 'n')
                this->nNormalDirection++;
        }
        else if (ch == 'f')
        {
            this->nFace++;
        }
        else if (ch == '\n')
        {
            continue;
        }
        LoadLine();
    }
    this->points = new Point[this->nPoint];
    this->textures = new Texture[max(1, this->nTexture)];
    this->normalDirections = new Point[this->nNormalDirection];
    this->faces = new Face[this->nFace << 1];
    this->materials = new Material[this->nMaterial];
}

void Parser::LoadData()
{
    char ch;

    freopen(this->mtlFileName.c_str(), "r", stdin);
    int materialIndex = -1;
    while (scanf("%c", &ch) != EOF)
    {
        if (ch == 'n')
        {
            scanf("%s", this->tempString);
            scanf("%s", this->tempString);
            materialIndex++;
            this->materials[materialIndex].name = string(this->tempString);
        }
        else if (ch == 'K')
        {
            scanf("%c", &ch);
            if (ch == 'a')
                scanf("%lf%lf%lf", &this->materials[materialIndex].ka.x, &this->materials[materialIndex].ka.y, &this->materials[materialIndex].ka.z);
            else if (ch == 'd')
                scanf("%lf%lf%lf", &this->materials[materialIndex].kd.x, &this->materials[materialIndex].kd.y, &this->materials[materialIndex].kd.z);
            else if (ch == 's')
                scanf("%lf%lf%lf", &this->materials[materialIndex].ks.x, &this->materials[materialIndex].ks.y, &this->materials[materialIndex].ks.z);
            else if (ch == 'e')
                scanf("%lf%lf%lf", &this->materials[materialIndex].ke.x, &this->materials[materialIndex].ke.y, &this->materials[materialIndex].ke.z);
        }
        else if (ch == 'T')
        {
            scanf("%c", &ch);
            if (ch == 'f')
                scanf("%lf%lf%lf", &this->materials[materialIndex].tf.x, &this->materials[materialIndex].tf.y, &this->materials[materialIndex].tf.z);
            else if (ch == 'r')
                scanf("%lf", &this->materials[materialIndex].tr);
        }
        else if (ch == 'N')
        {
            scanf("%c", &ch);
            if (ch == 's')
                scanf("%lf", &this->materials[materialIndex].ns);
            else if (ch == 'i')
                scanf("%lf", &this->materials[materialIndex].ni);
        }
        else if (ch == 'd')
            scanf("%lf", &this->materials[materialIndex].d);
        else if (ch == 'i')
            scanf("%s%d", this->tempString, &this->materials[materialIndex].illum);
#ifdef MAP_TEXTURE
        else if (ch == 'm')
        {
            scanf("%s", this->tempString);
            if (strcmp(this->tempString, "ap_Ka") == 0)
            {
                scanf("%s", this->tempString);
                string name = string(this->tempString);
                if (this->textureMap[name] != NULL)
                    this->materials[materialIndex].mapKa = this->textureMap[name];
                else
                    this->materials[materialIndex].mapKa = LoadTexture(name);
            }
            else if (strcmp(this->tempString, "ap_Kd") == 0)
            {
                scanf("%s", this->tempString);
                string name = string(this->tempString);
                if (this->textureMap[name] != NULL)
                    this->materials[materialIndex].mapKd = this->textureMap[name];
                else
                    this->materials[materialIndex].mapKd = LoadTexture(name);
            }
            else if (strcmp(this->tempString, "ap_Ks") == 0)
            {
                scanf("%s", this->tempString);
                string name = string(this->tempString);
                if (this->textureMap[name] != NULL)
                    this->materials[materialIndex].mapKs = this->textureMap[name];
                else
                    this->materials[materialIndex].mapKs = LoadTexture(name);
            }
        }
#endif
        else if (ch == '\n' || ch == ' ' || ch == '\t')
        {
            continue;
        }
        else
            LoadLine();
    }

    freopen(this->objFileName.c_str(), "r", stdin);
    int pointIndex = 0, textureIndex = 0, normalDirectionIndex = 0, faceIndex = 0;
    materialIndex = 0;
    while (scanf("%c", &ch) != EOF)
    {
        if (ch == 'v')
        {
            scanf("%c", &ch);
            if (ch == ' ')
            {
                scanf("%lf%lf%lf\n", &this->points[pointIndex].x, &this->points[pointIndex].y, &this->points[pointIndex].z);
                pointIndex++;
            }
            else if (ch == 't')
            {
                scanf("%lf%lf\n", &this->textures[textureIndex].u, &this->textures[textureIndex].v);
                textureIndex++;
            }
            else if (ch == 'n')
            {
                scanf("%lf%lf%lf\n", &this->normalDirections[normalDirectionIndex].x, &this->normalDirections[normalDirectionIndex].y, &this->normalDirections[normalDirectionIndex].z);
                normalDirectionIndex++;
            }
        }
        else if (ch == 'f')
        {
            scanf("%c", &ch);
            scanf("%c", &ch);
            int n = 0;
            while (ch >= '0' && ch <= '9')
            {
                this->faces[faceIndex].points[n].pointIndex = ch - '0';
                ReadInt(this->faces[faceIndex].points[n].pointIndex);
                int isEnd;
                scanf("%c", &ch);
                if (ch == '/')
                {
                    this->faces[faceIndex].points[n].textureIndex = 0;
                    this->faces[faceIndex].points[n].normalDirectionIndex = 0;
                    isEnd = ReadInt(this->faces[faceIndex].points[n].normalDirectionIndex);
                }
                else
                {
                    this->faces[faceIndex].points[n].textureIndex = ch - '0';
                    ReadInt(this->faces[faceIndex].points[n].textureIndex);
                    this->faces[faceIndex].points[n].normalDirectionIndex = 0;
                    isEnd = ReadInt(this->faces[faceIndex].points[n].normalDirectionIndex);
                }
                this->faces[faceIndex].points[n].pointIndex--;
                this->faces[faceIndex].points[n].textureIndex--;
                this->faces[faceIndex].points[n].normalDirectionIndex--;
                n++;
                if (!isEnd)
                    scanf("%c", &ch);
                else
                    break;
            }
            this->faces[faceIndex].nPoint = n;
            this->faces[faceIndex].material = &this->materials[materialIndex];
            if (n == 4)
            {
                int temp = faceIndex;
                Point a = sub(this->points[this->faces[faceIndex].points[1].pointIndex], this->points[this->faces[faceIndex].points[0].pointIndex]);
                Point b = sub(this->points[this->faces[faceIndex].points[2].pointIndex], this->points[this->faces[faceIndex].points[1].pointIndex]);
                Point x = mul(a, b);
                a = sub(this->points[this->faces[faceIndex].points[3].pointIndex], this->points[this->faces[faceIndex].points[2].pointIndex]);
                b = sub(this->points[this->faces[faceIndex].points[0].pointIndex], this->points[this->faces[faceIndex].points[3].pointIndex]);
                Point y = mul(a, b);
                Point z = mul(x, y);
                this->faces[faceIndex].normalDirection = x;
                normalization(this->faces[faceIndex].normalDirection);
                if ((isnan(this->faces[faceIndex].normalDirection.x) || isnan(this->faces[faceIndex].normalDirection.y) || isnan(this->faces[faceIndex].normalDirection.z)))
                    faceIndex--;
                if (!(eq(z.x, 0) && eq(z.y, 0) && eq(z.z, 0)) || (eq(x.x, 0) && eq(x.y, 0) && eq(x.z, 0)))
                {
                    this->faces[temp].nPoint = 3;
                    this->faces[faceIndex + 1].material = &this->materials[materialIndex];
                    this->faces[faceIndex + 1].nPoint = 3;
                    this->faces[faceIndex + 1].normalDirection = y;
                    normalization(this->faces[faceIndex + 1].normalDirection);
                    if (!(isnan(this->faces[faceIndex + 1].normalDirection.x) || isnan(this->faces[faceIndex + 1].normalDirection.y) || isnan(this->faces[faceIndex + 1].normalDirection.z)))
                    {
                        FacePoint tempPoint = this->faces[temp].points[0];
                        for (int i = 0; i < 2; i++)
                            this->faces[faceIndex + 1].points[i] = this->faces[temp].points[(i + 2) % 4];
                        this->faces[faceIndex + 1].points[2] = tempPoint;
                        faceIndex++;
                    }
                }
            }
            else if (n == 3)
            {
                Point a = sub(this->points[this->faces[faceIndex].points[1].pointIndex], this->points[this->faces[faceIndex].points[0].pointIndex]);
                Point b = sub(this->points[this->faces[faceIndex].points[2].pointIndex], this->points[this->faces[faceIndex].points[1].pointIndex]);
                this->faces[faceIndex].normalDirection = mul(a, b);
                normalization(this->faces[faceIndex].normalDirection);
                if ((isnan(this->faces[faceIndex].normalDirection.x) || isnan(this->faces[faceIndex].normalDirection.y) || isnan(this->faces[faceIndex].normalDirection.z)))
                    faceIndex--;
            }
            faceIndex++;
        }
        else if (ch == 'u')
        {
            scanf("%s", this->tempString);
            scanf("%s", this->tempString);
            materialIndex = this->materialMap[string(this->tempString)];
        }
        else if (ch == '\n')
        {
            continue;
        }
        else
        {
            LoadLine();
        }
    }
    this->nFace = faceIndex;

    if (_access(this->informationFileName.c_str(), 4) == 0)
    {
        this->hasCameraInformation = true;
        freopen(this->informationFileName.c_str(), "r", stdin);
        scanf("%lf%lf%lf", &this->cameraX.x, &this->cameraX.y, &this->cameraX.z);
        scanf("%lf%lf%lf", &this->cameraY.x, &this->cameraY.y, &this->cameraY.z);
        scanf("%lf%lf%lf", &this->cameraZ.x, &this->cameraZ.y, &this->cameraZ.z);
        scanf("%lf%lf%lf", &this->cameraO.x, &this->cameraO.y, &this->cameraO.z);
        scanf("%lf", &this->cameraPixelWidth);
        scanf("%lf", &this->cameraF);
        int flag;
        scanf("%d", &flag);
        this->cameraProjectionState = flag;
    }
    else
        this->hasCameraInformation = false;

#ifdef TEST
    printf("Point\n");
    for (int i = 0; i < this->nPoint; i++)
        printf("%.3f %.3f %.3f\n", this->points[i].x, this->points[i].y, this->points[i].z);
    printf("Texture\n");
    for (int i = 0; i < this->nTexture; i++)
        printf("%.3f %.3f\n", this->textures[i].u, this->textures[i].v);
    printf("NormalDirection\n");
    for (int i = 0; i < this->nNormalDirection; i++)
        printf("%.3f %.3f %.3f\n", this->normalDirections[i].x, this->normalDirections[i].y, this->normalDirections[i].z);
    printf("Face\n");
    for (int i = 0; i < this->nFace; i++)
    {
        for (int j = 0; j < this->faces[i].nPoint; j++)
            printf("%d/%d/%d ", this->faces[i].points[j].pointIndex, this->faces[i].points[j].textureIndex, this->faces[i].points[j].normalDirectionIndex);
        printf("%.3f %.3f %.3f\n", this->faces[i].normalDirection.x, this->faces[i].normalDirection.y, this->faces[i].normalDirection.z);
        printf("material=%d\n", this->faces[i].material);
    }
    printf("Material\n");
    for (int i = 0; i < this->nMaterial; i++)
    {
        printf("Ka %.3f %.3f %.3f\n", this->materials[i].ka.x, this->materials[i].ka.y, this->materials[i].ka.z);
        printf("Kd %.3f %.3f %.3f\n", this->materials[i].kd.x, this->materials[i].kd.y, this->materials[i].kd.z);
        printf("Ks %.3f %.3f %.3f\n", this->materials[i].ks.x, this->materials[i].ks.y, this->materials[i].ks.z);
        printf("Tf %.3f %.3f %.3f\n", this->materials[i].tf.x, this->materials[i].tf.y, this->materials[i].tf.z);
        printf("Ns=%.3f Ni=%.3f d=%.3f Tr=%.3f\n", this->materials[i].ns, this->materials[i].ni, this->materials[i].d, this->materials[i].tr);
        printf("illum=%d\n", this->materials[i].illum);
        printf("addr=%d\n", &this->materials[i]);
    }
#endif
}

void Parser::LoadLine()
{
    char ch;
    scanf("%c", &ch);
    while (ch != '\n')
        scanf("%c", &ch);
}

int Parser::ReadInt(int &x)
{
    char ch;
    scanf("%c", &ch);
    while (ch >= '0' && ch <= '9')
    {
        x = x * 10 + ch - '0';
        scanf("%c", &ch);
    }
    return ch == '\n';
}

#ifdef MAP_TEXTURE
QImage *Parser::LoadTexture(string name)
{
    string textureFileName = this->folderName + name;
    QImage *image = new QImage(textureFileName.c_str());
    this->textureMap[name] = image;
    return image;
}
#endif