// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "BLAStringID.h"
#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "BLASingleton.h"
#include "Buffer.h"

#define VertexAttributes(...) __VA_ARGS__
#define UniformValues(...) __VA_ARGS__

#define DeclareRenderPass(Name, VertexAttributes, UniformValues, attachmentCount) \
   typedef BLA::RenderPass<COMPILE_TIME_CRC32_STR(#Name), attachmentCount, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(VertexAttributes)>, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(UniformValues)>> Name;

#define RegisterRenderPass(Name)                                                                                        \
    class Name##Registrator                                                                                             \
    {                                                                                                                   \
    public:                                                                                                             \
        Name##Registrator()                                                                                             \
        {                                                                                                               \
            BLA::RenderPassRegistry* registry = BLA::RenderPassRegistry::GetSingletonInstance();                        \
            if(!registry)                                                                                               \
            {                                                                                                           \
                 registry = BLA::RenderPassRegistry::AssignAndReturnSingletonInstance(new RenderPassRegistry());        \
            }                                                                                                           \
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> vas = GetRenderPassVADescriptors<Name>();     \
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> uvs = GetRenderPassUVDescriptors<Name>();     \
            registry->__RegisterRenderPass(BlaStringId(#Name), Name::ms_renderPassId, 1, vas, uvs);                     \
        }                                                                                                               \
    };                                                                                                                  \
    static Name##Registrator ms_Name##Registrator;                                                  

namespace BLA
{
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
        const GPU::Buffer<blaU32>* m_indices;

        BaseRenderPassInstance(const GPU::Buffer<blaU32>& indices, blaU16 vaCount, blaU16 uvCount) :
            m_vaCount(vaCount), m_uvCount(uvCount), m_indices(&indices) {}

    public:
        void GetVertexAttribute(blaU32 i, const void*& pointerToData, blaSize& bufferLength)
        {
            //TODO fatal assert i <= vaCount

            const GPU::BaseBuffer* buffer = reinterpret_cast<const GPU::BaseBuffer*>(reinterpret_cast<blaU8*>(this + 1) + sizeof(const GPU::BaseBuffer*) * i);

            pointerToData = buffer->GetData();
            bufferLength = buffer->GetLength();
        }

        void GetUniformValuePtr(int i, const void*& pointerToData)
        {
            //TODO: fatal assert i <= uvCount

            pointerToData = reinterpret_cast<const void*>(reinterpret_cast<blaU8*>(this + 1) + sizeof(const GPU::BaseBuffer*) * m_vaCount + sizeof(const void*) * i);
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
        _RenderPassInstanceVAs(const GPU::Buffer<T>& dataVector, const GPU::Buffer<Ts>&... dataVectors) :
            _RenderPassInstanceVAs<Ts...>(dataVectors...),
            m_dataVector(&dataVector)
        {
            // TODO: Reason about type, statically check inspectability... Pre-fecth type information
            // Remember this constructor is executed every time we request a new RenderObject for this RenderPass, so not like it's only registered once yea ?
        };

        template <size_t k>
        const GPU::Buffer<typename std::enable_if<k == 0, T>::type>& GetVertexAttributes() const
        {
            return m_dataVector;
        }

        template <size_t k>
        const GPU::Buffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferVAType<k, _RenderPassInstanceVAs<T, Ts...>>::type>
            ::type>& GetVertexAttributes() const
        {
            return _RenderPassInstanceVAs<Ts...>::template GetVertexAttributes<k - 1>();
        }

    protected:
        const GPU::Buffer<T>* m_dataVector;
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

        _RenderPassInstance(const GPU::Buffer<blaU32>&indices, const InstanceVertexAttributes& vertexAttributes, const InstanceUniformValues& shaderUniforms ...):
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
        static_assert(attachmentCount > 0, "Number of attachments on a RenderPass < 1 is invalid");

        RenderPassAttachment m_attachment[attachmentCount];

    public:
        typedef _RenderPassInstance<RenderPass<renderPassId, attachmentCount, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>>,
                                        _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UVs...>> RenderPassInstance;

        static const blaU32 ms_renderPassId = renderPassId;
        static const size_t ms_VACount = sizeof...(VAs);
        static const size_t ms_UVCount = sizeof...(UVs);
    };

    template<int i, class RenderPass>
    class _GetRenderPassVADescriptorsInternal
    {
    public:
        static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassVADescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
            BLAInspectableVariables::ExposedVarTypeDescriptor* d =
                BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<i, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassVADescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            BLAInspectableVariables::ExposedVarTypeDescriptor* d =
                BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferVAType<0, typename RenderPass::RenderPassInstance::InstanceVertexAttributes>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassVADescriptors()
    {
        blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassVADescriptorsInternal<RenderPass::ms_VACount - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }

    template<int i, class RenderPass>
    class _GetRenderPassUVDescriptorsInternal
    {
    public:
        static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassVADescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
            BLAInspectableVariables::ExposedVarTypeDescriptor* d =
                BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<i, typename RenderPass::RenderPassInstance::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassUVDescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            BLAInspectableVariables::ExposedVarTypeDescriptor* d =
                BLAInspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUVType<0, typename RenderPass::RenderPassInstance::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    /*
     *  RenderPass templated accessors
     *
     */
    template<class RenderPass>
    blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassUVDescriptors()
    {
        blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassUVDescriptorsInternal<RenderPass::ms_UVCount - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }

    class RenderPassInstance
    {
        friend class Renderer;

        virtual void GetVertexAttribute(int i, void*& pointerToData, int& dataSize, int& bufferLength) = 0;
        virtual void GetUniformValuePtr(int i, void*& pointerToData, int& dataSize) = 0;
    };

    /*
     *  RenderPass Registry
     *
     */
    class RenderPassRegistry
    {
    public:
        struct Entry
        {
            blaStringId m_name;
            blaU32 m_attachmentCount;
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> m_vertexAttributesDescriptors;
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*> m_uniformValuesDescriptors;
        };

    private:
        BLA_DECLARE_EXPORTED_SINGLETON(RenderPassRegistry);

        typedef blaMap<blaU32, Entry> RenderPassRegistryStorage;
        RenderPassRegistryStorage m_registry;

    public:

        void __RegisterRenderPass(
            blaStringId name,
            blaU32 id,
            blaU32 attachmentCount,
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
            blaVector<BLAInspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor);

        BLACORE_API const Entry* GetRenderPassEntry(blaU32 id) const;

        BLACORE_API void GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const;
    };
}