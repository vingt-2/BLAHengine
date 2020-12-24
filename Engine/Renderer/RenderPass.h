// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "BLAStringID.h"
#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "BLASingleton.h"
#include "GPU/StaticBuffer.h"

#define VertexAttributes(...) __VA_ARGS__
#define UniformValues(...) __VA_ARGS__

#define DeclareRenderPass(Name, VertexAttributes, UniformValues, attachmentCount) \
   typedef BLA::RenderPass<COMPILE_TIME_CRC32_STR(#Name), attachmentCount, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(VertexAttributes)>, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(UniformValues)>> Name;

#define RegisterRenderPass(Name)                                                                                                \
    BLA_IMPLEMENT_CONSTRUCT_TEMPLATED_SINGLETON(Name, Name(BlaStringId(#Name)))

namespace BLA
{
    class BaseRenderPassImplementationDescriptor
    {
        
    };

    struct RenderPassDescriptor
    {
        blaStringId m_name;
        blaU32 m_attachmentCount;
        BaseRenderPassImplementationDescriptor* m_pToInstanceRenderPassDescriptorPointer;
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_vertexAttributesDescriptors;
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> m_uniformValuesDescriptors;
    };

    class BLACORE_API RenderPassAttachment
    {

    };

    namespace _RenderPassTemplateHelpers
    {
        // Dummy class to serve as Render Pass Input Type Separator (used to separate lists of variadic template arguments)
        template<typename...>
        class RPIS {};
    }

    class BaseRenderPassInstance
    {
        friend class Renderer;

        blaU16 m_vaCount;
        blaU16 m_uvCount;

    protected:
        const Gpu::StaticBuffer<blaU32>* m_indices;

        BaseRenderPassInstance(const Gpu::StaticBuffer<blaU32>& indices, blaU16 vaCount, blaU16 uvCount) :
            m_vaCount(vaCount), m_uvCount(uvCount), m_indices(&indices) {}

    public:
        void GetVertexAttribute(blaU32 i, const void*& pointerToData, blaSize& bufferLength)
        {
            //TODO fatal assert i <= vaCount

            const Gpu::BaseStaticBuffer* buffer = reinterpret_cast<const Gpu::BaseStaticBuffer*>(reinterpret_cast<blaU8*>(this + 1) + sizeof(const Gpu::BaseStaticBuffer*) * i);

            pointerToData = buffer->GetData();
            bufferLength = buffer->GetLength();
        }

        void GetUniformValuePtr(int i, const void*& pointerToData)
        {
            //TODO: fatal assert i <= uvCount

            pointerToData = reinterpret_cast<const void*>(reinterpret_cast<blaU8*>(this + 1) + sizeof(const Gpu::BaseStaticBuffer*) * m_vaCount + sizeof(const void*) * i);
        }
    };

    // Only inspectable types should be template arguments to a render pass.
    // Check and use it to infer things
    //
    template<typename... Ts>
    class _RenderPassInstanceVAs
    {
    public:
        _RenderPassInstanceVAs() {}
    };

    template<typename... Ts>
    class _RenderPassInstanceUVs
    {
    public:
        _RenderPassInstanceUVs<Ts...>() {};
    };

    namespace _RenderPassTemplateHelpers
    {
        template <size_t, class> struct InferVAType;

        template <typename T, typename... Ts>
        struct InferVAType<0, _RenderPassInstanceVAs<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferVAType<k, _RenderPassInstanceVAs<T, Ts...>>
        {
            typedef typename InferVAType<k - 1, _RenderPassInstanceVAs<Ts...>>::type type;
        };

        template <size_t, class> struct InferUVType;

        template <typename T, typename... Ts>
        struct InferUVType<0, _RenderPassInstanceUVs<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferUVType<k, _RenderPassInstanceUVs<T, Ts...>>
        {
            typedef typename InferUVType<k - 1, _RenderPassInstanceUVs<Ts...>>::type type;
        };
    }

    template<typename T, typename... Ts>
    class _RenderPassInstanceVAs<T, Ts...> : _RenderPassInstanceVAs<Ts...>
    {
    public:
        _RenderPassInstanceVAs(const Gpu::StaticBuffer<T>& dataVector, const Gpu::StaticBuffer<Ts>&... dataVectors) :
            _RenderPassInstanceVAs<Ts...>(dataVectors...),
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
        const Gpu::StaticBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferVAType<k, _RenderPassInstanceVAs<T, Ts...>>::type>
            ::type>& GetVertexAttributes() const
        {
            return _RenderPassInstanceVAs<Ts...>::template GetVertexAttributes<k - 1>();
        }

    protected:
        const Gpu::StaticBuffer<T>* m_dataVector;
    };

    template<typename T, typename... Ts>
    class _RenderPassInstanceUVs<T, Ts...> : public _RenderPassInstanceUVs<Ts...>
    {
    public:
        _RenderPassInstanceUVs(const T& uniformValue, const Ts&... uniformValues) :
            _RenderPassInstanceUVs<Ts...>(uniformValues...),
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
        const typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUVType<k, _RenderPassInstanceUVs<T, Ts...>>::type>::type& GetUniformValue() const
        {
            return _RenderPassInstanceUVs<Ts...>::template GetUniformValue<k - 1>();
        }

        template <size_t k>
        typename std::enable_if<k == 0, T>::type& GetUniformValue()
        {
            return m_uniformValue;
        }

        template <size_t k>
        typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUVType<k, _RenderPassInstanceUVs<T, Ts...>>::type>::type& GetUniformValue()
        {
            return _RenderPassInstanceUVs<Ts...>::template GetUniformValue<k - 1>();
        }

    protected:
        const T* m_uniformValue;
    };

    template<typename _RenderPass, class VertexAttributes, class ShaderUniforms>
    class _RenderPassInstance;

    template<typename _RenderPass, typename... VAs, typename... UVs>
    class _RenderPassInstance<_RenderPass, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>> : BaseRenderPassInstance, public _RenderPassInstanceVAs<VAs...>, public _RenderPassInstanceUVs<UVs...>
    {
    public:
        typedef _RenderPassInstanceVAs<VAs...> InstanceVertexAttributes;
        typedef _RenderPassInstanceUVs<UVs...> InstanceUniformValues;
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

        _RenderPassInstance(const Gpu::StaticBuffer<blaU32>&indices, const InstanceVertexAttributes& vertexAttributes, const InstanceUniformValues& shaderUniforms ...):
            BaseRenderPassInstance(indices, RenderPass::ms_VACount, RenderPass::ms_UVCount), _RenderPassInstanceVAs<VAs...>(vertexAttributes), _RenderPassInstanceUVs<UVs...>(shaderUniforms) {}
    };

    /*
     *   RenderPass definition
     *
     */
    template<blaU32 renderPassId, blaS32 attachmentCount, class VertexAttributes, class ShaderUniforms>
    class RenderPass;

    template<blaU32 renderPassId, blaS32 attachmentCount, typename... VAs, typename... UVs>
    class BLACORE_API RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>
    {
        using type = RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>;

        BLA_DECLARE_SINGLETON(type)

        static_assert(attachmentCount > 0, "Number of attachments on a RenderPass < 1 is invalid");

        RenderPassAttachment m_attachment[attachmentCount];
        const RenderPassDescriptor* m_pRenderPassDescriptor;

    public:
        typedef _RenderPassInstance<RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>,
                                        _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>> RenderPassInstance;

        //TODO: Dev builds only shoudl carry the blaStringid for the render pass constructor ...
        RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>,
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
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<i, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::type>::GetDescriptor();

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
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<0, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::type>::GetDescriptor();

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
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<i, typename RenderPass::RenderPassInstance::InstanceUniformValues>::type>::GetDescriptor();

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
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<0, typename RenderPass::RenderPassInstance::InstanceUniformValues>::type>::GetDescriptor();

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

        typedef blaMap<blaU32, RenderPassDescriptor> RenderPassRegistryStorage;
        RenderPassRegistryStorage m_registry;

    public:

        void __RegisterRenderPass(
            blaStringId name,
            blaU32 id,
            blaU32 attachmentCount,
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor);

        BLACORE_API const RenderPassDescriptor* GetRenderPassEntry(blaU32 id) const;

        BLACORE_API void GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const;
    };

    template <blaU32 renderPassId, blaS32 attachmentCount, typename ... VAs, typename ... UVs>
    RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS
        <UVs...>>::RenderPass(blaStringId id)
    {
        BLA::RenderPassRegistry* registry = BLA::RenderPassRegistry::GetSingletonInstance();
        if (!registry)
        {
            registry = BLA::RenderPassRegistry::AssignAndReturnSingletonInstance(new RenderPassRegistry());
        }
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> vas = GetRenderPassVADescriptors<type>();
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> uvs = GetRenderPassUVDescriptors<type>();
        registry->__RegisterRenderPass(id, ms_renderPassId, 1, vas, uvs);
        m_pRenderPassDescriptor = registry->GetRenderPassEntry(ms_renderPassId);
    }
}