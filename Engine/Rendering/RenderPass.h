// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "BLAStringID.h"
#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "BLASingleton.h"
#include "GPU/RenderPassAttachment.h"
#include "GPU/RenderPassDescription.h"
#include "GPU/StaticBuffer.h"
#include "GPU/DynamicBuffer.h"
#include "SillyMacros.h"
#include "ProjectExport.h"

#define VertexAttributes(...) __VA_ARGS__
#define UniformValues(...) __VA_ARGS__

#define DeclareRenderPass(Name, RenderPassAttachment, VertexAttributes, UniformValues) \
   typedef BLA::RenderPass<COMPILE_TIME_CRC32_STR(#Name), RenderPassAttachment, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(VertexAttributes)>, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(UniformValues)>> Name;

#define RegisterRenderPass(Name) \
    BLA_IMPLEMENT_CONSTRUCT_TEMPLATED_SINGLETON(Name, Name(BlaStringId(#Name)))

namespace BLA
{
    namespace _RenderPassTemplateHelpers
    {
        // Dummy class to serve as Render Pass Input Type Separator (used to separate lists of variadic template arguments)
        template<typename...>
        class RPIS {};
    }

    class BaseRenderPassObject
    {
        friend class Renderer;
    
    protected:
        
        BaseRenderPassObject(const Gpu::StaticBuffer<blaU32>& indices, blaU16 vaCount, blaU16 uvCount) :
            m_vaCount(vaCount), m_uvCount(uvCount), m_indices(&indices) {}

    public:
        
        void GetVertexAttributeBuffer(blaU32 i, const Gpu::BaseStaticBuffer*& buffer) const
        {
            //TODO fatal assert i <= vaCount

            buffer = *reinterpret_cast<const Gpu::BaseStaticBuffer**>((blaU8*)(this + 1) + sizeof(const Gpu::BaseStaticBuffer**) * i);
        }


        // Really should  be a dynamic buffer ...
        void GetUniformValueBuffer(int i, const Gpu::BaseDynamicBuffer*& buffer) const
        {
            //TODO: fatal assert i <= uvCount

            buffer = *reinterpret_cast<const Gpu::BaseDynamicBuffer**>((blaU8*)(this + 1) + sizeof(const Gpu::BaseStaticBuffer**) * m_vaCount + 8 /*Holy shit that's a huge hack. Gotta find a less retarded way to address each buffer*/
                + sizeof(const Gpu::BaseDynamicBuffer**) * i);
        }

        const blaU16 m_vaCount;
        const blaU16 m_uvCount;

        const Gpu::StaticBuffer<blaU32>* m_indices;
    };

    // Only inspectable types should be template arguments to a render pass.
    // Check and use it to infer things
    //
    template<typename... Ts>
    class _RenderPassObjectVAs
    {
    public:
        _RenderPassObjectVAs() {}
    };

    template<typename... Ts>
    class _RenderPassObjectUVs
    {
    public:
        _RenderPassObjectUVs<Ts...>() {};
    };

    namespace _RenderPassTemplateHelpers
    {
        template <size_t, class> struct InferVAType;

        template <typename T, typename... Ts>
        struct InferVAType<0, _RenderPassObjectVAs<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferVAType<k, _RenderPassObjectVAs<T, Ts...>>
        {
            typedef typename InferVAType<k - 1, _RenderPassObjectVAs<Ts...>>::type type;
        };

        template <size_t, class> struct InferUVType;

        template <typename T, typename... Ts>
        struct InferUVType<0, _RenderPassObjectUVs<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferUVType<k, _RenderPassObjectUVs<T, Ts...>>
        {
            typedef typename InferUVType<k - 1, _RenderPassObjectUVs<Ts...>>::type type;
        };
    }

    template<typename T, typename... Ts>
    class _RenderPassObjectVAs<T, Ts...> : _RenderPassObjectVAs<Ts...>
    {
    public:
        _RenderPassObjectVAs(const Gpu::StaticBuffer<T>& dataVector, const Gpu::StaticBuffer<Ts>&... dataVectors) :
            _RenderPassObjectVAs<Ts...>(dataVectors...),
            m_dataVector(&dataVector)
        {
            // TODO: statically check type compatibility
            // Remember this constructor is executed every time we request a new RenderObject for this RenderPass, so not like it's only registered once yea ?
        };

        template <size_t k>
        const Gpu::StaticBuffer<typename std::enable_if<k == 0, T>::type>& GetVertexAttributes() const
        {
            return m_dataVector;
        }

        template <size_t k>
        const Gpu::StaticBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferVAType<k, _RenderPassObjectVAs<T, Ts...>>::type>
            ::type>& GetVertexAttributes() const
        {
            return _RenderPassObjectVAs<Ts...>::template GetVertexAttributes<k - 1>();
        }

    protected:
        const Gpu::StaticBuffer<T>* m_dataVector;
    };

    template<typename T, typename... Ts>
    class _RenderPassObjectUVs<T, Ts...> : public _RenderPassObjectUVs<Ts...>
    {
    public:
        _RenderPassObjectUVs(const Gpu::DynamicBuffer<T>& uniformValue, const Gpu::DynamicBuffer<Ts>&... uniformValues) :
            _RenderPassObjectUVs<Ts...>(uniformValues...),
            m_uniformValue(&uniformValue)
        {
            // TODO: Reason about type, statically check inspectability... Pre-fecth type information
            // Remember this constructor is executed every time we request a new RenderObject for this RenderPass, so not like it's only registered once yea ?
        };

        template <size_t k>
        const typename std::enable_if<k == 0, T>::type& GetUniformValue() const
        {
            return m_uniformValue;
        }

        template <size_t k>
        const Gpu::DynamicBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUVType<k, _RenderPassObjectUVs<T, Ts...>>::type>::type>& GetUniformValue() const
        {
            return _RenderPassObjectUVs<Ts...>::template GetUniformValue<k - 1>();
        }

        template <size_t k>
        typename std::enable_if<k == 0, T>::type& GetUniformValue()
        {
            return m_uniformValue;
        }

        template <size_t k>
        Gpu::DynamicBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUVType<k, _RenderPassObjectUVs<T, Ts...>>::type>::type>& GetUniformValue()
        {
            return _RenderPassObjectUVs<Ts...>::template GetUniformValue<k - 1>();
        }

    protected:
        const Gpu::DynamicBuffer<T>* m_uniformValue;
    };

    template<typename _RenderPass, class VertexAttributes, class ShaderUniforms>
    class _RenderPassObject;

    template<typename _RenderPass, typename... VAs, typename... UVs>
    class _RenderPassObject<_RenderPass, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>> : BaseRenderPassObject, public _RenderPassObjectVAs<VAs...>, public _RenderPassObjectUVs<UVs...>
    {
        friend class Gpu::Interface;

    public:
        typedef _RenderPassObjectVAs<VAs...> InstanceVertexAttributes;
        typedef _RenderPassObjectUVs<UVs...> InstanceUniformValues;
        typedef _RenderPass RenderPass;

        template<int n>
        struct GetVAType
        {
            typedef typename _RenderPassTemplateHelpers::InferVAType<n, InstanceVertexAttributes>::type Type;
        };

        template<int n>
        struct GetUVType
        {
            typedef typename _RenderPassTemplateHelpers::InferUVType<n, InstanceUniformValues>::type Type;
        };

        _RenderPassObject(const Gpu::StaticBuffer<blaU32>&indices, const InstanceVertexAttributes& vertexAttributes, const InstanceUniformValues& shaderUniforms):
            BaseRenderPassObject(indices, RenderPass::ms_VACount, RenderPass::ms_UVCount), _RenderPassObjectVAs<VAs...>(vertexAttributes), _RenderPassObjectUVs<UVs...>(shaderUniforms) {}
    };

    /*
     *   RenderPass definition
     *
     */
    template<blaU32 renderPassId, class RenderPassAttachment, class VertexAttributes, class ShaderUniforms>
    class RenderPass;

    template<blaU32 renderPassId, class RenderPassAttachment, typename... VAs, typename... UVs>
    class RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>
    {
        static_assert(!std::is_base_of<Gpu::BaseRenderPassAttachment, RenderPassAttachment>::value, 
            "Invalid Render Pass Attachment passed to RenderPass. Did you not declare it with DeclareRenderPassAttachment ?");

        using type = RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>;

        BLA_DECLARE_SINGLETON(type);

        RenderPassAttachment m_attachment;

    public:
        const Gpu::RenderPassDescriptor* m_pRenderPassDescriptor;

        typedef _RenderPassObject<RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>,
                                        _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>> RenderPassObject;

        //TODO: Dev builds only shoudl carry the blaStringid for the render pass constructor ...
        RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>,
                   _RenderPassTemplateHelpers::RPIS<UVs...>>(blaStringId id);

        static const blaU32 ms_renderPassId = renderPassId;
        static const size_t ms_VACount = sizeof...(VAs);
        static const size_t ms_UVCount = sizeof...(UVs);
    };

    template<int i, class RenderPass>
    class _GetRenderPassVADescriptorsInternal
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassVADescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<i, typename RenderPass::RenderPassObject::InstanceVertexAttributes>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassVADescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<0, typename RenderPass::RenderPassObject::InstanceVertexAttributes>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassVADescriptors()
    {
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassVADescriptorsInternal<RenderPass::ms_VACount - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }

    template<int i, class RenderPass>
    class _GetRenderPassUVDescriptorsInternal
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassVADescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<i, typename RenderPass::RenderPassObject::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassUVDescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<0, typename RenderPass::RenderPassObject::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    /*
     *  RenderPass templated accessors
     *
     */
    template<class RenderPass>
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassUVDescriptors()
    {
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassUVDescriptorsInternal<RenderPass::ms_UVCount - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }

    /*
     *  RenderPass Registry
     *
     */
    class RenderPassRegistry
    {
        BLA_DECLARE_EXPORTED_SINGLETON(RenderPassRegistry);

        typedef blaMap<blaU32, Gpu::RenderPassDescriptor> RenderPassRegistryStorage;
        RenderPassRegistryStorage m_registry;

    public:

        BLACORE_API void __RegisterRenderPass(
            blaStringId name,
            blaU32 id,
            Gpu::RPAttachmentDescription attachmentDescription,
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor);

        BLACORE_API Gpu::RenderPassDescriptor* GetRenderPassEntry(blaU32 id);

        BLACORE_API const Gpu::RenderPassDescriptor* GetRenderPassEntry(blaU32 id) const;

        BLACORE_API void GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const;
    };

    template <blaU32 renderPassId, class RenderPassAttachment, typename ... VAs, typename ... UVs>
    inline RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS
                      <UVs...>>::RenderPass(blaStringId id)
    {
        BLA::RenderPassRegistry* registry = BLA::RenderPassRegistry::GetSingletonInstance();
        if (!registry)
        {
            registry = BLA::RenderPassRegistry::AssignAndReturnSingletonInstance(new RenderPassRegistry());
        }
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> vas = GetRenderPassVADescriptors<type>();
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> uvs = GetRenderPassUVDescriptors<type>();

        Gpu::RPAttachmentDescription attachmentDescription;
        registry->__RegisterRenderPass(id, ms_renderPassId, attachmentDescription, vas, uvs);
        m_pRenderPassDescriptor = registry->GetRenderPassEntry(ms_renderPassId);
    }
}