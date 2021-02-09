// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

namespace BLA
{
    namespace Gpu
    {
        struct Opaque
        {
            enum class Type
            {
                Sampler,
            };

            Opaque(Type type): m_type(type) {}

            Type GetType() const { return m_type; }

        private:
            Type m_type;
        };

        struct BaseTexture;
        struct Sampler : Opaque
        {
            Sampler(BaseTexture* texture) : Opaque(Type::Sampler), m_texture(texture) {}
            const BaseTexture* m_texture;
        };
    }
};