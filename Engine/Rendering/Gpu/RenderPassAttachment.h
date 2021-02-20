// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "Rendering/Gpu/Image.h"

#define ColorAttachments(...) __VA_ARGS__

#define DeclareRenderPassAttachment(Name, ColorAttachments, DepthAttachment) \
   typedef BLA::Gpu::RenderPassAttachment<BLA::Gpu::_RenderPassAttachmentTemplateHelpers::CATS<EXPAND(ColorAttachments)>, DepthAttachment> Name;


// TODO:
// It'd be nice to move the should clear decision to compile time templating ... That is because
// I originally wanted to have 1 c++ type == 1 VKRenderPass descriptor
// and right now you could set a different clear parameter to subsequent attachment assignments ... Which would not reflect the change ...
// Not good 

namespace BLA
{
    namespace Gpu
    {
        namespace _RenderPassAttachmentTemplateHelpers
        {
            // Dummy class to serve as Color Attachment Type separator (used to separate lists of variadic template arguments)
            template<typename...>
            class CATS {};

            template <size_t, class> struct InferCAT;
        }

        struct BaseRenderPassAttachment
        {
            BaseRenderPassAttachment(blaU16 colorAttachmentCount) : m_colorCount(colorAttachmentCount) {}

            struct ClearSetting
            {
                blaVec4 m_clearValue;
                bool m_shouldClear = false;
            };

            struct UntypedAttachmentDesc
            {
                ClearSetting m_clear;
                Gpu::BaseImage* m_image = nullptr;
            };

            void GetColorAttachment(blaU32 i, UntypedAttachmentDesc& untypedAttachmentDesc) const
            {
                //TODO fatal assert i <= m_colorCount
                untypedAttachmentDesc = *reinterpret_cast<const UntypedAttachmentDesc*>((blaU8*)(this + 1) + sizeof(UntypedAttachmentDesc) * i);
            }

            blaU16 m_colorCount;

            UntypedAttachmentDesc m_depth;
        };

        template<typename AttachmentFormat>
        struct ColorAttachmentDesc
        {
            ColorAttachmentDesc(Gpu::Image<AttachmentFormat>* image) : m_image(image) {}

            ColorAttachmentDesc(Gpu::Image<AttachmentFormat>* image, float redClearValue, float greenClearValue, float blueClearValue, float alphaClearValue) :
                m_image(image), m_clearSettings({ {redClearValue , greenClearValue, blueClearValue, alphaClearValue}, true }) {}

            BaseRenderPassAttachment::ClearSetting m_clearSettings;

            Gpu::Image<AttachmentFormat>* m_image;
        };

        template<typename AttachmentFormat>
        struct DepthStencilAttachmentDesc
        {
            DepthStencilAttachmentDesc(Gpu::Image<AttachmentFormat>* image) : m_image(image) {}

            DepthStencilAttachmentDesc(Gpu::Image<AttachmentFormat>* image, float depthClearValue, float stencilClearValue): m_image(image), m_clearSettings({ {depthClearValue , stencilClearValue, 0.f, 0.f}, true }) {}

            BaseRenderPassAttachment::ClearSetting m_clearSettings;

            Gpu::Image<AttachmentFormat>* m_image;
        };


        template<typename... Ts>
        class _RenderPassAttachmentCATs
        {
        public:
            _RenderPassAttachmentCATs<Ts...>() {};
        };

        namespace _RenderPassAttachmentTemplateHelpers
        {
            template <typename T, typename... Ts>
            struct InferCAT<0, _RenderPassAttachmentCATs<T, Ts...>>
            {
                typedef T type;
            };

            template <size_t k, typename T, typename... Ts>
            struct InferCAT<k, _RenderPassAttachmentCATs<T, Ts...>>
            {
                typedef typename InferCAT<k - 1, _RenderPassAttachmentCATs<Ts...>>::type type;
            };
        }

        template<typename T, typename... Ts>
        class _RenderPassAttachmentCATs<T, Ts...> : public _RenderPassAttachmentCATs<Ts...>
        {
        public:
            _RenderPassAttachmentCATs(const ColorAttachmentDesc<T>& attachmentDesc, const ColorAttachmentDesc<Ts>&... attachmentDescs) :
                _RenderPassAttachmentCATs<Ts...>(attachmentDescs...), m_colorAttachment(attachmentDesc) {};

            /*
             * Const accessor to Color Attachment
             */
            template <size_t k>
            const ColorAttachmentDesc<typename std::enable_if<k == 0, T>::type>& GetColorAttachment() const
            {
                return m_colorAttachment;
            }

            template <size_t k>
            const ColorAttachmentDesc<typename std::enable_if<k != 0, typename _RenderPassAttachmentTemplateHelpers::InferCAT<k, _RenderPassAttachmentCATs<T, Ts...>>::type>::type>& GetColorAttachment() const
            {
                return _RenderPassAttachmentCATs<Ts...>::template GetColorAttachment<k - 1>();
            }

            /*
             * Non-Const Accessor to Color Attachment
             */
            template <size_t k>
            ColorAttachmentDesc<typename std::enable_if<k == 0, T>::type>& GetColorAttachment()
            {
                return m_colorAttachment;
            }

            template <size_t k>
            ColorAttachmentDesc<typename std::enable_if<k != 0, typename _RenderPassAttachmentTemplateHelpers::InferCAT<k, _RenderPassAttachmentCATs<T, Ts...>>::type>::type>& GetColorAttachment()
            {
                return _RenderPassAttachmentCATs<Ts...>::template GetColorAttachment<k - 1>();
            }

        protected:
            const ColorAttachmentDesc<T> m_colorAttachment;
        };

        template<typename DepthFormat>
        struct _DepthStencilAttachment
        {
            _DepthStencilAttachment(const DepthStencilAttachmentDesc<DepthFormat>& depthAttachmentDesc) : m_depthStencilAttachment(depthAttachmentDesc) {}

            DepthStencilAttachmentDesc<DepthFormat> m_depthStencilAttachment;
        };

        template<>
        class _DepthStencilAttachment<void> {};

        template<class ColorAttachmentTypes, typename DepthAttachmentType = void>
        struct RenderPassAttachment;

        template<typename _DepthAttachmentType, typename... CATs>
        struct RenderPassAttachment<_RenderPassAttachmentTemplateHelpers::CATS<CATs...>, _DepthAttachmentType> : BaseRenderPassAttachment,
            _RenderPassAttachmentCATs<CATs...>, _DepthStencilAttachment<_DepthAttachmentType>
        {
            typedef _RenderPassAttachmentCATs<CATs...> Color;
            typedef _DepthAttachmentType Depth;

            template<typename U = _DepthAttachmentType>
            RenderPassAttachment(const Color& colorAttachments, typename std::enable_if<std::is_same<U, void>::value>::type* = 0) : BaseRenderPassAttachment(ms_colorAttachmentCounts), 
                _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthStencilAttachment<void>()
            {}

            template<typename U = _DepthAttachmentType>
            RenderPassAttachment(const Color& colorAttachments, const DepthStencilAttachmentDesc<_DepthAttachmentType>& depthAttachment, typename std::enable_if<!std::is_same<U, void>::value>::type* = 0):
                BaseRenderPassAttachment(ms_colorAttachmentCounts), _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthStencilAttachment<_DepthAttachmentType>(depthAttachment)
            {
                static_assert(sizeof(UntypedAttachmentDesc) == sizeof(DepthStencilAttachmentDesc<_DepthAttachmentType>));

                memcpy_s(&m_depth, sizeof(UntypedAttachmentDesc), &depthAttachment, sizeof(DepthStencilAttachmentDesc<_DepthAttachmentType>));
            }

            static const size_t ms_colorAttachmentCounts = sizeof...(CATs);
        };

        template<int i, class Attachment>
        class _GetColorAttachmentDescriptorsInternal
        {
        public:
            static void Get(blaVector<Formats::Enum::Index>& typeDescriptors)
            {
                _GetColorAttachmentDescriptorsInternal<i - 1, Attachment>::Get(typeDescriptors);
                const Formats::Enum::Index format = 
                    typename _RenderPassAttachmentTemplateHelpers::InferCAT<i, typename Attachment::Color>::type::ms_formatIndex;

                typeDescriptors.push_back(format);
            }
        };

        template<class Attachment>
        class _GetColorAttachmentDescriptorsInternal<0, Attachment>
        {
        public:
            static void Get(blaVector<Formats::Enum::Index>& typeDescriptors)
            {
                const Formats::Enum::Index format =
                    typename _RenderPassAttachmentTemplateHelpers::InferCAT<0, typename Attachment::Color>::type::ms_formatIndex;

                typeDescriptors.push_back(format);
            }
        };

        template<class Attachment>
        blaVector<Formats::Enum::Index> GetColorAttachmentDescriptors()
        {
            blaVector<Formats::Enum::Index> formats;

            _GetColorAttachmentDescriptorsInternal<Attachment::ms_colorAttachmentCounts - 1, Attachment>::Get(formats);

            return formats;
        }

        template<class Attachment>
        Formats::Enum::Index _GetDepthAttachmentDescriptionInternal(typename std::enable_if<std::is_same<typename Attachment::Depth, void>::value, void>::type*)
        {
            return Formats::Enum::Index::INVALID;
        }


        template<class Attachment>
        Formats::Enum::Index _GetDepthAttachmentDescriptionInternal(typename std::enable_if<!std::is_same<typename Attachment::Depth, void>::value, void>::type*)
        {
            return Attachment::Depth::ms_formatIndex;
        }

        template<class Attachment>
        Formats::Enum::Index GetDepthAttachmentDescription()
        {
            return _GetDepthAttachmentDescriptionInternal<Attachment>(nullptr);
        }
    }
}