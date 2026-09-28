#include <iostream>
#include <windows.h>
#include <vector>
#include <chrono>
#include <fstream>
#include <math.h>

int screen_width = 120;
int screen_hight = 80;


class Vector3f
{
    public:
    float x;
    float y;
    float z;
    Vector3f(float x_in = 0 , float y_in = 0, float z_in = 0)
    {
        x = x_in;
        y = y_in;
        z = z_in;
    };
    Vector3f operator!=(const Vector3f& other) const
    {
        Vector3f result;
        
        result.x = this->x != other.x;
        result.y = this->y != other.y;
        result.z = this->z != other.z;
        return result;
    } 
    Vector3f operator==(const Vector3f& other) const
    {
        Vector3f result;
        
        result.x = this->x == other.x;
        result.y = this->y == other.y;
        result.z = this->z == other.z;
        return result;
    } 
    Vector3f operator+(const Vector3f& other) const
    {
        Vector3f result;
        
        result.x = this->x + other.x;
        result.y = this->y + other.y;
        result.z = this->z + other.z;
        return result;
    } 
    Vector3f operator-(const Vector3f& other) const
    {
        Vector3f result;
        
        result.x = this->x - other.x;
        result.y = this->y - other.y;
        result.z = this->z - other.z;
        return result;
    } 
};
using namespace std;

const float PI = 3.14159265f;


Vector3f value = Vector3f(0, 0, 0);
Vector3f camera_pos = Vector3f(0, 0, 0);
Vector3f camera_pos2 = Vector3f(0, 0, 0);
vector<Vector3f> obj_ver;
vector<Vector3f> tri_ver_call;
Vector3f ver_tri;
Vector3f ver_tri2;
Vector3f ver_tri3;
Vector3f angle2;

int debug = -1;
int debug_value = -1;
bool c_spawn = false;

int line = 0;

void map_file_read();
Vector3f rotate( Vector3f position , float angle_x , float angle_y, float angle_z);
void rendering_cube(const int x_max, const int y_max, const int z_max, Vector3f postion, int angle_x, int angle_y, int angle_z);
void camera_postion(Vector3f relative_pos, Vector3f object_postion, Vector3f relative_rotation);
void corrdinates_calculations(float x, float y, float z);
void objectbool();
void debug_trigger();
void line_algorthim(Vector3f p1, Vector3f p2);
void triangle_draw(Vector3f p1, Vector3f p2,Vector3f p3);

Vector3f cube_pos[60];
Vector3f cube_scale[60];
Vector3f cube_rotation[60];
int object_current = 1;
int object_count = 20;
Vector3f c_rotation = Vector3f(0, 0, 0);

wchar_t *screen = new wchar_t [screen_hight*screen_width];


int main()
{
    
    HANDLE hBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    SetConsoleActiveScreenBuffer(hBuffer);
    DWORD bytesWritten = 0;
    auto inital_time = chrono::system_clock::now();
    auto current_time = chrono::system_clock::now();
    for(int h = 0; h < screen_width * screen_hight; h++)
    {
        screen[h] = L' ';
    }


    while (1)
    {
        current_time = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = current_time - inital_time;
        float fElapsedTime = elapsedTime.count();
        inital_time = current_time;

        if (GetAsyncKeyState((unsigned short)'O') & 1 )
        {
            cout << "debug Mode activated!\n";
            debug_value = -debug_value;
        }
        if (GetAsyncKeyState((unsigned short)' ') & 1 ) 
        {
            cout << "spawn_cube\n";
            c_spawn = true;
        }
        if(GetAsyncKeyState((unsigned short)'V') & 1 )
        {
            map_file_read();
            cout << "file_updated" << endl;
        }
        if(GetAsyncKeyState((unsigned short)'Z') & 1 )
        {
            object_current = 0;
            cout << "spawing_reset" << endl;
        }
        if(GetAsyncKeyState((unsigned short)'X') & 1 )
        {
            for(int i = 0; i < object_count; i++)
            {
                cube_pos[i] = Vector3f(0, 0, 0);
                cube_rotation[i] = Vector3f(0, 0, 0);
                cube_scale[i] = Vector3f(0, 0, 0);
            }
        }

        if (GetAsyncKeyState((unsigned short)VK_UP) & 0x8000 ) camera_pos.z = 0.0002;
        else if (GetAsyncKeyState((unsigned short)VK_DOWN) & 0x8000) camera_pos.z = -0.0002;
        else camera_pos.z = 0;

        if (GetAsyncKeyState((unsigned short)VK_LEFT) & 0x8000 ) camera_pos.x = -0.0002;
        else if (GetAsyncKeyState((unsigned short)VK_RIGHT) & 0x8000) camera_pos.x = 0.0002;
        else camera_pos.x = 0;

        if (GetAsyncKeyState((unsigned short)'R') & 0x8000 ) value.x += fElapsedTime * 60;
        if (GetAsyncKeyState((unsigned short)'F') & 0x8000 ) value.x -= fElapsedTime * 60;

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000 ) value.y += fElapsedTime * 60;
        if (GetAsyncKeyState((unsigned short)'G') & 0x8000 ) value.y -= fElapsedTime * 60;

        if (GetAsyncKeyState((unsigned short)'E') & 0x8000 ) value.z += fElapsedTime * 60;
        if (GetAsyncKeyState((unsigned short)'T') & 0x8000 ) value.z -= fElapsedTime * 60;
        
        debug_trigger();
        objectbool();
        map_file_read();
        screen[screen_width * screen_hight - 1] = L'\0';
        WriteConsoleOutputCharacterW(hBuffer, screen, screen_width * screen_hight, {0,0}, &bytesWritten);      
        for(int h = 0; h < screen_width * screen_hight; h++)
        {
            screen[h] = L' ';
        }
    }
}

void map_file_read()
{
    fstream file;
    file.open("map.obj", ios::in);

    if(file.is_open())
    {       
        string current_line;
        string current_line1;
        string current_line2;
        string current_line3;
        Vector3f ver;
        while(getline(file, current_line))
        { 
            if (current_line[0] == 'v' && current_line[1] == ' ')
            {
                current_line1 = current_line.substr(2, current_line.length());
                ver.x = stof(current_line1.substr(0, current_line1.find_first_of(' ') - 1));
                
                current_line2 = current_line1.substr(current_line1.find_first_of(' ') + 1, current_line1.length());
                ver.y = stof(current_line2.substr(0, current_line2.find_first_of(' ') - 1));
                current_line3 = current_line2.substr(current_line2.find_first_of(' ') + 1, current_line2.length());
                ver.z = stof(current_line3.substr(0, current_line3.find_first_of(' ') - 1));
                obj_ver.push_back(ver);
            }
            if(current_line[0] == 'f' && current_line[1] == ' ')
            {
                current_line1 = current_line.substr(2, current_line.length());
                
                ver_tri = obj_ver[stof(current_line1.substr(0, current_line1.find_first_of(' '))) - 1];

                
                current_line2 = current_line1.substr(current_line1.find_first_of(' ')+ 1, current_line1.length());
                ver_tri2 = obj_ver[stof(current_line2.substr(0, current_line2.find_first_of(' '))) - 1];

                current_line3 = current_line2.substr(current_line2.find_first_of(' ') + 1, current_line2.length());
                ver_tri3 = obj_ver[stof(current_line3.substr(0, current_line3.find_first_of(' '))) - 1];
                triangle_draw(ver_tri, ver_tri2, ver_tri3);
            } 
            
            
        }
        file.close();
    }
}
void line_algorthim(Vector3f p1, Vector3f p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    float maximum2 = max(abs(dx), abs(dy));
    float maximum = max(maximum2, abs(dz));
    float maximum_x = dx / maximum;
    float maximum_y = dy / maximum;
    float maximum_z = dz / maximum;
    for(int i = 0; i < maximum; i++)
    {
        Vector3f result =  rotate(Vector3f(p1.x + (i * maximum_x), p1.y + (i * maximum_y), p1.z + (i * maximum_z)), angle2.x, angle2.y, angle2.z);
        camera_postion (camera_pos2, result, value);
    }
}
void triangle_draw(Vector3f p1, Vector3f p2,Vector3f p3)
{
    line_algorthim(p1, p2);
    line_algorthim(p2, p3);
    line_algorthim(p3, p1);
}

void rotation_debug_animation()
{
    Vector3f pos = rotate(Vector3f(0, 0, 50), value.x, value.y, value.z);
    cube_pos[0] =  pos + camera_pos2;
    cube_scale[0] = Vector3f(20, 20, 0);
}
void rotation_debug_animation_end()
{
    cube_pos[0] =  Vector3f(0, 0, 0);
    cube_scale[0] = Vector3f(0, 0, 0);
}
void debug_mode()
{

   

    if (c_spawn)
    { 
        Vector3f pos = rotate(Vector3f(0, 0, 50), value.x, value.y, value.z);
        cube_pos[object_current] = pos + camera_pos2;
        cube_scale[object_current] = Vector3f(20, 20, 0);
        object_current++;
        c_spawn = false;
    }
    
    if (GetAsyncKeyState((unsigned short)'L') & 0x8000 )
    { 
        cube_rotation[0].x++; 
        angle2.x++;
        rotation_debug_animation();
    }
    if (GetAsyncKeyState((unsigned short)VK_OEM_COMMA) & 0x8000)
    { 
        cube_rotation[0].x--; 
        angle2.x--;
    }

    if (GetAsyncKeyState((unsigned short)'K') & 0x8000 )
    { 
        cube_rotation[0].y++; 
        angle2.y++;
    }
    if (GetAsyncKeyState((unsigned short)'J') & 0x8000 )
    { 
        cube_rotation[0].y--;
        angle2.y--;
    }

    if (GetAsyncKeyState((unsigned short)'M') & 0x8000 )
    {
        cube_rotation[0].z++;
        angle2.z++;
    }
    if (GetAsyncKeyState((unsigned short)'N') & 0x8000 )
    {
        cube_rotation[0].z--;
        angle2.z--;
    }
    if(GetAsyncKeyState((unsigned short)'H') & 0x8000 )
    {
        rotation_debug_animation();
    }
    else
    {
        rotation_debug_animation_end();
    }


    cube_rotation[object_current] = cube_rotation[0];

    

}
void debug_trigger()
{
    debug = debug_value;
    
    if(debug == 1)
    {
        debug_mode();
    }
}

Vector3f rotate( Vector3f position , float angle_x , float angle_y, float angle_z)
{
    const float theta = angle_x * PI / 180.0f; 
    const float cos_theta = cos(theta);
    const float sin_theta = sin(theta);
    
    const float beta = angle_y * PI / 180.0f; 
    const float cos_beta = cos(beta);
    const float sin_beta = sin(beta);
    
    const float delta = angle_z * PI / 180.0f; 
    const float cos_delta = cos(delta);
    const float sin_delta = sin(delta);

    const float X =  position.x * (cos_delta * cos_beta) + position.y * (sin_delta * cos_beta) + position.z * -(sin_beta);

    const float Y =  position.x * (cos_delta * sin_beta * sin_theta - sin_delta * cos_theta) + position.y * (sin_delta * sin_beta * sin_theta + cos_delta * cos_theta) + position.z * (cos_beta * sin_theta);

    const float Z =  position.x * (cos_delta * sin_beta * cos_theta + sin_delta * sin_theta) + position.y * (sin_delta * sin_beta * cos_theta - cos_delta * sin_theta) + position.z * (cos_beta * cos_theta);

    return Vector3f(X, Y, Z);
}

void camera_postion(Vector3f relative_pos, Vector3f object_postion, Vector3f relative_rotation)
{ 
    Vector3f result =  rotate(camera_pos,relative_rotation.x, relative_rotation.y, 0);
    
     camera_pos2 = camera_pos2 + result;

    Vector3f pos =  object_postion - relative_pos;
    
    Vector3f result2 =  rotate(pos, -relative_rotation.x, -relative_rotation.y, -relative_rotation.z);
    
    corrdinates_calculations(result2.x, result2.y, result2.z);
}

void corrdinates_calculations(float x, float y, float z)
{
    
    float screen_z = 70; 
    int screen_x = ((x * screen_z) / z) + screen_width / 2;
    int screen_y = (((y * screen_z) / z) / 2) + screen_hight / 2;

    if(screen_x >= screen_width || screen_x < 0 || screen_y >= screen_hight || screen_y < 0 || z < 0) return;  

    screen[screen_y * screen_width + screen_x] = L'.'; //█;
}
void rendering_cube(const int x_max, const int y_max, const int z_max, Vector3f postion, int angle_x, int angle_y, int angle_z)
{
    for(int x = -x_max; x <= x_max; x++)
    {
        for(int y = -y_max; y <= y_max; y++)
        {      
            for(int z = -z_max; z <= z_max; z++)
            {
                Vector3f result =  rotate(Vector3f(x, y, z), angle_x, angle_y, angle_z);
                result = result + postion;
                camera_postion(camera_pos2, result, value);
                
            }
        }   
    }

}

void objectbool()
{
    for(int i = 0; i < object_count; i++)
    {
        rendering_cube(cube_scale[i].x, cube_scale[i].y, cube_scale[i].z, cube_pos[i], cube_rotation[i].x, cube_rotation[i].y, cube_rotation[i].z);
    }   
}
