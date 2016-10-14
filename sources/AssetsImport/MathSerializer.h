#include "../Common/Maths.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\xml.hpp>

namespace BLAengine
{
	class vec3serializer
	{
	public:

		vec3serializer() = default;

		void FillData(vec3 &vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		void LoadData(vec3 &vec)
		{
			vec.x = x;
			vec.y = y;
			vec.z = z;
		}

		float x, y, z;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				x, y, z
			);
		}
	};

	class vec2serializer
	{
	public:
		vec2serializer() = default;

		void FillData(vec2 &vec)
		{
			x = vec.x;
			y = vec.y;
		}

		void LoadData(vec2 &vec)
		{
			vec.x = x;
			vec.y = y;
		}

		float x, y;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				x, y
			);
		}
	};

	class mat3serializer
	{
	public:

		mat3serializer() = default;

		void FillData(mat3 &mat)
		{
			a = mat[0][0];
			b = mat[0][1];
			c = mat[0][2];
			d = mat[1][0];
			e = mat[1][1];
			f = mat[1][2];
			g = mat[2][0];
			h = mat[2][1];
			i = mat[2][2];
		}

		void LoadData(mat3 &mat)
		{
			mat[0][0] = a;
			mat[0][1] = b;
			mat[0][2] = c;
			mat[1][0] = d;
			mat[1][1] = e;
			mat[1][2] = f;
			mat[2][0] = g;
			mat[2][1] = h;
			mat[2][2] = i;

		}

		float a, b, c, d, e, f, g, h, i;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				a, b, c, d, e, f, g, h, i
			);
		}
	};

}