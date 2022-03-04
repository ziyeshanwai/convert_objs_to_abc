// Alembic Includes
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

// Other includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <string>
#include <Vector>
#include <io.h>
#include <algorithm>
// We include some global mesh data to test with from an external source
// to keep this example code clean.

using namespace std;
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract


struct vertice
{
	float x;
	float y;
	float z;
};

struct uv
{
	float u;
	float v;
};

void ReadObj(string path, std::vector<vertice>& OutVertices, std::vector<uv>& OutUvs, std::vector<vertice>& OutNormals, std::vector<int>& OutFace, std::vector<int>& Outgcounts)
{
	string s, str, s1, s2, s3, s4, s5, s6;
	ifstream inf;
	vertice v;
	vertice normal;
	uv u;
	string::size_type idx;
	inf.open(path);

	int vn = 0;
	int vnum = 0;
	int fnum = 0;
	if (inf.is_open())
	{
		while (getline(inf, s))
		{

			//std::cout << s << std::endl;
			if (s[0] == 'v')
			{
				if (s[1] == 't')
				{
					istringstream in(s);
					in >> s1 >> s2 >> s3;
					u.u = std::stof(s2);
					u.v = std::stof(s3);
					OutUvs.push_back(u);
				}
				else if (s[1] == 'n')
				{
					istringstream in(s);
					in >> s1 >> s2 >> s3 >> s4;
					normal.x = std::stof(s2);
					normal.y = std::stof(s3);
					normal.z = std::stof(s4);
					OutNormals.push_back(normal);
				}
				else
				{
					istringstream in(s);
					in >> s1 >> s2 >> s3 >> s4;
					v.x = std::stof(s2);
					v.y = std::stof(s3);
					v.z = std::stof(s4);
					OutVertices.push_back(v);
				}
			}

			int a = 0, b = 0, c = 0, d = 0;
			if (s[0] == 'f')
			{
				istringstream in(s);
				in >> s1 >> s2 >> s3 >> s4 >> s5;
				idx = s1.find("/");
				if (idx == string::npos) // no exist '/'
				{
					
					a = std::stoi(s2);
					OutFace.push_back(a - 1);
					b = std::stoi(s3);
					OutFace.push_back(b - 1);
					c = std::stoi(s4);
					OutFace.push_back(c - 1);
					if (!s5.empty())
					{
						d = std::stoi(s5);
						OutFace.push_back(d - 1);
						Outgcounts.push_back(4);
					}
					else
					{
						Outgcounts.push_back(3);
					}
				}
				else
				{
					for (int k = 0; s2[k] != '/'; k++)
					{
						a = a * 10 + (s2[k] - 48);
					}
					OutFace.push_back(a - 1);
					for (int k = 0; s3[k] != '/'; k++)
					{
						b = b * 10 + (s3[k] - 48);
					}
					OutFace.push_back(b - 1);
					for (int k = 0; s4[k] != '/'; k++)
					{
						c = c * 10 + (s4[k] - 48);
					}
					OutFace.push_back(c - 1);
					if (!s5.empty())
					{
						for (int k = 0; s5[k] != '/'; k++)
						{
							d = d * 10 + (s5[k] - 48);
						}
						OutFace.push_back(d - 1);
						Outgcounts.push_back(4);
					}
					else
					{
						Outgcounts.push_back(3);
					}
				}
				

			}

		}

	}

	inf.close();

}


void getFiles(string path, std::vector<string>& files)
{
	//文件句柄
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


void seq2abc(string inputdir, string ouputfile, float fps)
{
	string suffixStr = "obj";
	std::vector<vertice> Vertices;
	std::vector<vertice> Normals;
	std::vector<uv> Uvs;
	std::vector<int> face;
	std::vector<string> filenames;
	std::vector<int> g_counts_array;
	getFiles(inputdir, filenames);
	int i = 0;
	OArchive archive(Alembic::AbcCoreOgawa::WriteArchive(), ouputfile);
	TimeSamplingPtr ts(new TimeSampling(1.0 / fps, 0.0));
	OPolyMesh meshyObj(OObject(archive, kTop), "mymesh", ts);
	OPolyMeshSchema& mesh = meshyObj.getSchema();
	ReadObj(filenames[0], Vertices, Uvs, Normals, face, g_counts_array);
	std::vector< V3f > verts(Vertices.size());
	for (size_t i = 0; i < Vertices.size(); ++i)
	{
		verts[i] = V3f(Vertices[i].x, Vertices[i].y, Vertices[i].z);
	}
	int32_t g_numIndices2 = face.size();
	int32_t* g_indices2 = new int32_t[g_numIndices2];
	//int32_t g_indices[g_numIndices];
	std::copy(face.begin(), face.end(), g_indices2);
	Abc::int32_t* g_counts2 = new Abc::int32_t[g_counts_array.size()];
	std::copy(g_counts_array.begin(), g_counts_array.end(), g_counts2);
	OPolyMeshSchema::Sample mesh_samp(
		V3fArraySample(verts),
		Int32ArraySample(g_indices2, g_numIndices2),
		Int32ArraySample(g_counts2, g_counts_array.size()));

	mesh.set(mesh_samp);
	for (int i = 1; i < filenames.size(); i++)
	{
		Vertices.clear();
		face.clear();
		Uvs.clear();
		Normals.clear();
		string filetype = filenames[i].substr(filenames[i].find_last_of(".") + 1);
		transform(filetype.begin(), filetype.end(), filetype.begin(), ::tolower);

		if (filetype == suffixStr)
		{
			ReadObj(filenames[i], Vertices, Uvs, Normals, face, g_counts_array);

		}
		for (size_t i = 0; i < Vertices.size(); ++i)
		{
			verts[i] = V3f(Vertices[i].x, Vertices[i].y, Vertices[i].z);
		}

		std::cout << "verteice size " << Vertices.size() << std::endl;
		std::cout << "face count " << face.size() << std::endl;
		std::cout << "read name " << filenames[i] << std::endl;
		mesh.set(mesh_samp);

	}

	/*
	ON3fGeomParam::Sample nsamp( N3fArraySample( (const N3f *)g_normals,
		g_numNormals ), kFacevaryingScope );
	mesh_samp.setNormals( nsamp );

	OV2fGeomParam::Sample uvsamp( V2fArraySample( (const V2f *)g_uvs,
								  g_numUVs ), kFacevaryingScope );
	mesh_samp.setUVs( uvsamp );
	mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
										   g_numVerts ) );
	mesh.set( mesh_samp );

	mesh_samp.setNormals( ON3fGeomParam::Sample() );
	mesh_samp.setUVs( OV2fGeomParam::Sample() );
	mesh_samp.setVelocities( V3fArraySample() );
	mesh.set( mesh_samp );


	mesh_samp.setVelocities( V3fArraySample( ( const V3f * )g_veloc,
										   g_numVerts ) );
	mesh_samp.setUVs( uvsamp );
	mesh_samp.setNormals( nsamp );
	*/

	/*
	mesh_samp.setUVs( OV2fGeomParam::Sample() );
	mesh_samp.setNormals( ON3fGeomParam::Sample() );
	mesh_samp.setVelocities( V3fArraySample() );
	mesh.set( mesh_samp );
	*/



	/*
	IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name );

	IPolyMesh meshyObj( IObject( archive, kTop ), "mesh" );
	IPolyMeshSchema &mesh = meshyObj.getSchema();
	TESTING_ASSERT( 7 == mesh.getNumSamples() );
	TESTING_ASSERT( 7 == mesh.getVelocitiesProperty().getNumSamples() );
	TESTING_ASSERT( 7 == mesh.getUVsParam().getNumSamples() );
	TESTING_ASSERT( 7 == mesh.getNormalsParam().getNumSamples() );
	TESTING_ASSERT(
		GetSourceName( mesh.getUVsParam().getMetaData() ) == "" );
	TESTING_ASSERT( isUV( mesh.getUVsParam().getHeader() ) );
	*/

}

void print_usage(const char* name)
{
	std::cout << "\nUsage: " << name << " [options]" << std::endl
		<< "Options:" << std::endl
		<< "  -h, --help     help  \n"
		<< "  -i, --in       obj input dir \n"
		<< "  -o, --out      output abc name \n"
		<< "  -f --fps       abc frame rate \n"
		<< "\n"
		<< std::endl;
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		print_usage(argv[0]);
		return 1;
	}
	string inputdir = "E:\\CPP_Project\\Objs2Abc\\x64\\Release\\total";
	string output = "test.abc";
	float fps = 24.0;
	
	for (int i = 1; i < argc; i++)
	{
		std::string t_arg = std::string(argv[i]);
		if (t_arg == "-h" || t_arg == "--help")
		{
			print_usage(argv[0]);
			return 0;
		}
		else if (t_arg == "-i" || t_arg == "--in")
		{
			inputdir = argv[i + 1];
		}
		else if (t_arg == "-o" || t_arg == "--out")
		{
			output = argv[i + 1];
		}
		else if (t_arg == "-f" || t_arg == "--fps")
		{
			fps = std::stof(argv[i + 1]);
		}
		
	}
	
	std::cout << "input dir: " << inputdir << std::endl;
	std::cout << "output abc: " << output << std::endl;
	std::cout << "fps: " << fps << std::endl;

	// Mesh out
	
	seq2abc(inputdir, output, fps);

	return 0;
}