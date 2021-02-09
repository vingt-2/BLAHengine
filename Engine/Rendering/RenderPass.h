// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "BLAStringID.h"
#include "StdInclude.h"
#include "Core/InspectableVariables.h"
#include "BLASingleton.h"
#include "Gpu/RenderPassAttachment.h"
#include "Gpu/RenderPassDescription.h"
#include "Gpu/RenderPassProgram.h"
#include "Gpu/StaticBuffer.h"
#include "Gpu/DynamicBuffer.h"
#include "Gpu/Opaques.h"
#include "SillyMacros.h"
#include "ProjectExport.h"

#define VertexAttributes(...) __VA_ARGS__
#define UniformBufferObjects(...) __VA_ARGS__
#define OpaqueUniforms(...) __VA_ARGS__

#define DeclareRenderPass(Name, RenderPassAttachment, VertexAttributes, UniformBufferObjects, OpaqueUniforms) \
   typedef BLA::RenderPass<COMPILE_TIME_CRC32_STR(#Name), RenderPassAttachment, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(VertexAttributes)>, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(UniformBufferObjects)>, BLA::_RenderPassTemplateHelpers::RPIS<EXPAND(OpaqueUniforms)>> Name;

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

    // TODO: Change me ... 
    using DefaultTODOToSpecifyRenderProgram = Gpu::RenderPassProgram;

    class BaseRenderPassObject
    {
        friend class Renderer;
    
    protected:
        
        BaseRenderPassObject(const Gpu::StaticBuffer<blaU32>& indices, blaU16 vaCount, blaU16 uvCount, blaU16 opaqueCount) :
            m_vaCount(vaCount), m_uvCount(uvCount), m_opaqueCount(opaqueCount), m_indices(&indices) {}

    public:
        
        void GetVertexAttributeBuffer(blaU32 i, const Gpu::BaseStaticBuffer*& buffer) const
        {
            //TODO fatal assert i <= vaCount

            buffer = *reinterpret_cast<const Gpu::BaseStaticBuffer**>((blaU8*)(this + 1) + sizeof(const Gpu::BaseStaticBuffer**) * i);
        }

        // Really should  be a dynamic buffer ...
        void GetUniformBufferObjectBuffer(int i, const Gpu::BaseDynamicBuffer*& buffer) const
        {
            //TODO: fatal assert i <= uvCount
            //TODO:
            ///*Holy shit that's a huge hack. Gotta find a less retarded way to address each buffer*/
            //TODO:
            //TODO:

            buffer = *reinterpret_cast<const Gpu::BaseDynamicBuffer**>((blaU8*)(this + 1) + sizeof(const Gpu::BaseStaticBuffer**) * m_vaCount + 8 /*<---- Holy shit that's a huge hack. Gotta find a less retarded way to address each buffer*/
                + sizeof(const Gpu::BaseDynamicBuffer**) * i);
        }

        // Really should  be a dynamic buffer ...
        void GetOpaqueValue(int i, const Gpu::Opaque*& opaque) const
        {
            //TODO: fatal assert i <= uvCount
            //TODO:
            ///*Holy shit that's a huge hack. Gotta find a less retarded way to address each buffer*/
            //TODO:
            //TODO:

            opaque = *reinterpret_cast<const Gpu::Opaque**>((blaU8*)(this + 1) + sizeof(const Gpu::BaseStaticBuffer**) * m_vaCount + 8 + sizeof(const Gpu::BaseStaticBuffer**) * m_uvCount + 8 /*<---- Holy shit that's a huge hack. Gotta find a less retarded way to address each buffer*/
                + sizeof(const Gpu::Opaque**) * i);
        }

        //TODO: Optimize ! This shouldnt be more than a u32
        const blaU16 m_vaCount;
        const blaU16 m_uvCount;
        const blaU16 m_opaqueCount;

        const Gpu::StaticBuffer<blaU32>* m_indices;
    };

    struct BaseRenderPassInstance
    {
        BaseRenderPassInstance(blaU32 typeId) : m_pImplementation(nullptr), m_instanceTypeId(typeId) {}

        virtual ~BaseRenderPassInstance() = default;

        Gpu::RenderPassInstanceImplementation* m_pImplementation;

        const blaU32 m_instanceTypeId;

    protected:

        void Setup(DefaultTODOToSpecifyRenderProgram& program);
        void BaseResetAttachment(const Gpu::BaseRenderPassAttachment* rpAttachment) const;
        void BaseRegisterRenderPassObject(const BaseRenderPassObject& object) const;

        const Gpu::RenderPassDescriptor* m_pRenderPassDescriptor;
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
    class _RenderPassObjectUBOs
    {
    public:
        _RenderPassObjectUBOs<Ts...>() {};
    };

    template<typename... Ts>
    class _RenderPassObjectOpaques
    {
    public:
        _RenderPassObjectOpaques<Ts...>() {};
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

        template <size_t, class> struct InferUBOType;

        template <typename T, typename... Ts>
        struct InferUBOType<0, _RenderPassObjectUBOs<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferUBOType<k, _RenderPassObjectUBOs<T, Ts...>>
        {
            typedef typename InferUBOType<k - 1, _RenderPassObjectUBOs<Ts...>>::type type;
        };

        template <size_t, class> struct InferOpaqueType;

        template <typename T, typename... Ts>
        struct InferOpaqueType<0, _RenderPassObjectOpaques<T, Ts...>>
        {
            typedef T type;
        };

        template <size_t k, typename T, typename... Ts>
        struct InferOpaqueType<k, _RenderPassObjectOpaques<T, Ts...>>
        {
            typedef typename InferUBOType<k - 1, _RenderPassObjectUBOs<Ts...>>::type type;
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
    class _RenderPassObjectUBOs<T, Ts...> : public _RenderPassObjectUBOs<Ts...>
    {
    public:
        _RenderPassObjectUBOs(const Gpu::DynamicBuffer<T>& uniformValue, const Gpu::DynamicBuffer<Ts>&... uniformValues) :
            _RenderPassObjectUBOs<Ts...>(uniformValues...),
            m_uniformValue(&uniformValue)
        {
            // TODO: Reason about type, statically check inspectability... Pre-fecth type information
            // Remember this constructor is executed every time we request a new RenderObject for this RenderPass, so not like it's only registered once yea ?
        };

        template <size_t k>
        Gpu::DynamicBuffer<const typename std::enable_if<k == 0, T>::type>& GetUniformValue() const
        {
            return m_uniformValue;
        }

        template <size_t k>
        const Gpu::DynamicBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUBOType<k, _RenderPassObjectUBOs<T, Ts...>>::type>::type>& GetUniformValue() const
        {
            return _RenderPassObjectUBOs<Ts...>::template GetUniformValue<k - 1>();
        }

        template <size_t k>
        Gpu::DynamicBuffer<typename std::enable_if<k == 0, T>::type>& GetUniformValue()
        {
            return m_uniformValue;
        }

        template <size_t k>
        Gpu::DynamicBuffer<typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUBOType<k, _RenderPassObjectUBOs<T, Ts...>>::type>::type>& GetUniformValue()
        {
            return _RenderPassObjectUBOs<Ts...>::template GetUniformValue<k - 1>();
        }

    protected:
        const Gpu::DynamicBuffer<T>* m_uniformValue;
    };

    template<typename T, typename... Ts>
    class _RenderPassObjectOpaques<T, Ts...> : public _RenderPassObjectOpaques<Ts...>
    {
    public:
        _RenderPassObjectOpaques(const T& opaqueValue, const Ts&... opaqueValues) :
            _RenderPassObjectOpaques<Ts...>(opaqueValues...),
            m_opaqueValue(opaqueValue)
        {
            // TODO: Reason about type, statically check inspectability... Pre-fecth type information
            // Remember this constructor is executed every time we request a new RenderObject for this RenderPass, so not like it's only registered once yea ?
        };

        template <size_t k>
        const typename std::enable_if<k == 0, T>::type& GetOpaqueValue() const
        {
            return m_opaqueValue;
        }

        template <size_t k>
        const typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUBOType<k, _RenderPassObjectOpaques<T, Ts...>>::type>::type& GetOpaqueValue() const
        {
            return _RenderPassObjectOpaques<Ts...>::template GetOpaqueValue<k - 1>();
        }

        template <size_t k>
        typename std::enable_if<k == 0, T>::type& GetOpaqueValue()
        {
            return m_opaqueValue;
        }

        template <size_t k>
        typename std::enable_if<k != 0, typename _RenderPassTemplateHelpers::InferUBOType<k, _RenderPassObjectOpaques<T, Ts...>>::type>::type& GetOpaqueValue()
        {
            return _RenderPassObjectOpaques<Ts...>::template GetOpaqueValue<k - 1>();
        }

    protected:
        T m_opaqueValue;
    };

    template<typename _RenderPass, class VertexAttributes, class ShaderUniforms, class OpaqueValues>
    class _RenderPassObject;

    template<typename _RenderPass, typename... VAs, typename... UBOs, typename... Opaques>
    class _RenderPassObject<_RenderPass, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>> :
        public BaseRenderPassObject, public _RenderPassObjectVAs<VAs...>, public _RenderPassObjectUBOs<UBOs...>, public _RenderPassObjectOpaques<Opaques...>
    {
        friend class Gpu::Interface;

    public:
        typedef _RenderPassObjectVAs<VAs...> InstanceVertexAttributes;
        typedef _RenderPassObjectUBOs<UBOs...> InstanceUniformValues;
        typedef _RenderPassObjectOpaques<Opaques...> InstanceOpaqueValues;

        typedef _RenderPass RenderPass;

        template<int n>
        struct GetVAType
        {
            typedef typename _RenderPassTemplateHelpers::InferVAType<n, InstanceVertexAttributes>::type Type;
        };

        template<int n>
        struct GetUBOType
        {
            typedef typename _RenderPassTemplateHelpers::InferUBOType<n, InstanceUniformValues>::type Type;
        };

        template<int n>
        struct GetOpaqueType
        {
            typedef typename _RenderPassTemplateHelpers::InferOpaqueType<n, InstanceUniformValues>::type Type;
        };

        _RenderPassObject(const Gpu::StaticBuffer<blaU32>&indices, const InstanceVertexAttributes& vertexAttributes, const InstanceUniformValues& shaderUniforms, const InstanceOpaqueValues& opaqueValues):
            BaseRenderPassObject(indices, RenderPass::ms_VACount, RenderPass::ms_UBOCount, RenderPass::ms_OpaquesCount), _RenderPassObjectVAs<VAs...>(vertexAttributes), _RenderPassObjectUBOs<UBOs...>(shaderUniforms), _RenderPassObjectOpaques<Opaques...>(opaqueValues) {}
    };

    template<typename _RenderPass, typename _RenderPassAttachment, typename _RenderPassRenderProgram>
    struct _RenderPassInstance : BaseRenderPassInstance
    {
        typedef _RenderPass RenderPass;
        typedef _RenderPassAttachment Attachment;
        typedef _RenderPassRenderProgram RenderProgram;

        void ResetAttachment(const Attachment& attachment)
        {
            const Gpu::BaseRenderPassAttachment* renderPassAttachmentAsBase = static_cast<const Gpu::BaseRenderPassAttachment*>(&attachment);
            BaseResetAttachment(renderPassAttachmentAsBase);
        }

        _RenderPassInstance(const Attachment& attachment, const RenderProgram& renderProgram) : BaseRenderPassInstance(_RenderPass::ms_renderPassId), m_attachment(attachment), m_renderProgram(renderProgram)
        {
            m_pRenderPassDescriptor = RenderPass::GetSingletonInstanceRead()->m_pRenderPassDescriptor;

            Setup(m_renderProgram);
            ResetAttachment(attachment);
        }

        void RegisterRenderPassObject(const typename RenderPass::RenderPassObject& object)
        {
            BaseRegisterRenderPassObject(object);
        }

        Attachment m_attachment;
        RenderProgram m_renderProgram;
    };

    /*
     *  TODO: Remove this being a singleton and have m_pRenderPassDescritpor be static
     *   RenderPass definition
     *
     */
    template<blaU32 renderPassId, class RenderPassAttachment, class VertexAttributes, class ShaderUniforms, class OpaqueValues>
    class RenderPass;

    template<blaU32 renderPassId, class _RenderPassAttachment, typename... VAs, typename... UBOs, typename... Opaques>
    class RenderPass<renderPassId, _RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>>
    {
        static_assert(std::is_base_of<Gpu::BaseRenderPassAttachment, _RenderPassAttachment>::value, 
            "Invalid Render Pass Attachment passed to RenderPass. Did you not declare it with DeclareRenderPassAttachment ?");

        using SelfType = RenderPass<renderPassId, _RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>>;

        BLA_DECLARE_SINGLETON(SelfType);

    public:
        
        typedef _RenderPassAttachment RenderPassAttachment;
        typedef _RenderPassObject<SelfType, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS<UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>> RenderPassObject;
        typedef _RenderPassInstance<SelfType, _RenderPassAttachment, DefaultTODOToSpecifyRenderProgram> RenderPassInstance;

        //TODO: Dev builds only shoudl carry the blaStringid for the render pass constructor ...
        RenderPass<renderPassId, _RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>,
                   _RenderPassTemplateHelpers::RPIS<UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>>(blaStringId id);

        const Gpu::RenderPassDescriptor* m_pRenderPassDescriptor;

        static const blaU32 ms_renderPassId = renderPassId;
        static const size_t ms_VACount = sizeof...(VAs);
        static const size_t ms_UBOCount = sizeof...(UBOs);
        static const size_t ms_OpaquesCount = sizeof...(Opaques);
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
    class _GetRenderPassVADescriptorsInternal<-1, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& /*typeDescriptors*/)
        {
            static_assert(false, "A RenderPass must have at least on vertex attribute type to valid");
        }
    };

    template<class RenderPass>
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassVADescriptors()
    {
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassVADescriptorsInternal< static_cast<int>(RenderPass::ms_VACount) - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }

    template<int i, class RenderPass>
    class _GetRenderPassUBODescriptorsInternal
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassUBODescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUBOType<i, typename RenderPass::RenderPassObject::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassUBODescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<typename _RenderPassTemplateHelpers::InferUBOType<0, typename RenderPass::RenderPassObject::InstanceUniformValues>::type>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassUBODescriptorsInternal<-1, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& /*typeDescriptors*/) {}
    };

    /*
     *  RenderPass templated accessors
     *
     */
    template<class RenderPass>
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassUBODescriptors()
    {
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassUBODescriptorsInternal<static_cast<int>(RenderPass::ms_UBOCount) - 1, RenderPass>::Get(typeDescriptors);

        return typeDescriptors;
    }


    template<int i, class RenderPass>
    class _GetRenderPassOpaqueDescriptorsInternal
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            _GetRenderPassOpaqueDescriptorsInternal<i - 1, RenderPass>::Get(typeDescriptors);

            using OpaqueType = typename _RenderPassTemplateHelpers::InferOpaqueType<i, typename RenderPass::RenderPassObject::InstanceOpaqueValues>::type;

            static_assert(std::is_base_of<Gpu::Opaque, OpaqueType>::value, "Invalid Opaque type");

            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<OpaqueType>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassOpaqueDescriptorsInternal<0, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& typeDescriptors)
        {
            using OpaqueType = typename _RenderPassTemplateHelpers::InferOpaqueType<0, typename RenderPass::RenderPassObject::InstanceOpaqueValues>::type;

            static_assert(std::is_base_of<Gpu::Opaque, OpaqueType>::value, "Invalid Opaque type");

            BLA::Core::InspectableVariables::ExposedVarTypeDescriptor* d =
                BLA::Core::InspectableVariables::TypeResolver<OpaqueType>::GetDescriptor();

            typeDescriptors.push_back(d);
        }
    };

    template<class RenderPass>
    class _GetRenderPassOpaqueDescriptorsInternal<-1, RenderPass>
    {
    public:
        static void Get(blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& /*typeDescriptors*/) {}
    };

    /*
     *  RenderPass templated accessors
     *
     */
    template<class RenderPass>
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> GetRenderPassOpaqueDescriptors()
    {
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> typeDescriptors;

        _GetRenderPassOpaqueDescriptorsInternal<static_cast<int>(RenderPass::ms_OpaquesCount) - 1, RenderPass>::Get(typeDescriptors);

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
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptors,
            blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& opaqueDescriptors);

        BLACORE_API Gpu::RenderPassDescriptor* GetRenderPassEntry(blaU32 id);

        BLACORE_API const Gpu::RenderPassDescriptor* GetRenderPassEntry(blaU32 id) const;

        BLACORE_API void GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const;
    };

    template <blaU32 renderPassId, class RenderPassAttachment, typename ... VAs, typename ... UBOs, typename... Opaques>
    inline RenderPass<renderPassId, RenderPassAttachment, _RenderPassTemplateHelpers::RPIS<VAs...>, _RenderPassTemplateHelpers::RPIS
                      <UBOs...>, _RenderPassTemplateHelpers::RPIS<Opaques...>>::RenderPass(blaStringId id)
    {
        BLA::RenderPassRegistry* registry = BLA::RenderPassRegistry::GetSingletonInstance();
        if (!registry)
        {
            registry = BLA::RenderPassRegistry::AssignAndReturnSingletonInstance(new RenderPassRegistry());
        }
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> vas = GetRenderPassVADescriptors<SelfType>();
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> uvs = GetRenderPassUBODescriptors<SelfType>();
        blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*> opaques = GetRenderPassOpaqueDescriptors<SelfType>();

        const Gpu::RPAttachmentDescription attachmentDescription = 
            { Gpu::GetColorAttachmentDescriptors<RenderPassAttachment>(), Gpu::GetDepthAttachmentDescription<RenderPassAttachment>() };

        registry->__RegisterRenderPass(id, ms_renderPassId, attachmentDescription, vas, uvs, opaques);
        m_pRenderPassDescriptor = registry->GetRenderPassEntry(ms_renderPassId);
    }
}