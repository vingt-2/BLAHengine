// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

#include "Rendering/Gpu/Image.h"

#define ColorAttachments(...) __VA_ARGS__

#define DeclareRenderPassAttachment(Name, ColorAttachments, DepthAttachment) \
   typedef BLA::Gpu::RenderPassAttachment<BLA::Gpu::_RenderPassAttachmentTemplateHelpers::CATS<EXPAND(ColorAttachments)>, DepthAttachment> Name;

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

        struct BaseImage;

        struct BaseRenderPassAttachment {};

        template<typename AttachmentFormat>
        struct AttachmentDesc
        {
            AttachmentDesc(Gpu::Image<AttachmentFormat> image) : m_image(image) {}

            Gpu::Image<AttachmentFormat> m_image;
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
            _RenderPassAttachmentCATs(const AttachmentDesc<T>& attachmentDesc, const AttachmentDesc<Ts>&... attachmentDescs) :
                _RenderPassAttachmentCATs<Ts...>(attachmentDescs...), m_colorAttachment(attachmentDesc) {};

            /*
             * Const accessor to Color Attachment
             */
            template <size_t k>
            const AttachmentDesc<typename std::enable_if<k == 0, T>::type>& GetColorAttachment() const
            {
                return m_colorAttachment;
            }

            template <size_t k>
            const AttachmentDesc<typename std::enable_if<k != 0, typename _RenderPassAttachmentTemplateHelpers::InferCAT<k, _RenderPassAttachmentCATs<T, Ts...>>::type>::type>& GetColorAttachment() const
            {
                return _RenderPassAttachmentCATs<Ts...>::template GetColorAttachment<k - 1>();
            }

            /*
             * Non-Const Accessor to Color Attachment
             */
            template <size_t k>
            AttachmentDesc<typename std::enable_if<k == 0, T>::type>& GetColorAttachment()
            {
                return m_colorAttachment;
            }

            template <size_t k>
            AttachmentDesc<typename std::enable_if<k != 0, typename _RenderPassAttachmentTemplateHelpers::InferCAT<k, _RenderPassAttachmentCATs<T, Ts...>>::type>::type>& GetColorAttachment()
            {
                return _RenderPassAttachmentCATs<Ts...>::template GetColorAttachment<k - 1>();
            }

        protected:
            const AttachmentDesc<T> m_colorAttachment;
        };

        template<typename DepthFormat>
        struct _DepthAttachment
        {
            _DepthAttachment(const AttachmentDesc<DepthFormat>& depthAttachmentDesc) : m_depthAttachment(depthAttachmentDesc) {}

            AttachmentDesc<DepthFormat> m_depthAttachment;
        };

        template<>
        class _DepthAttachment<void> {};

        template<class ColorAttachmentTypes, typename DepthAttachmentType = void>
        struct RenderPassAttachment;

        template<typename DepthAttachmentType, typename... CATs>
        struct RenderPassAttachment<_RenderPassAttachmentTemplateHelpers::CATS<CATs...>, DepthAttachmentType> :
            _RenderPassAttachmentCATs<CATs...>, _DepthAttachment<DepthAttachmentType>, BaseRenderPassAttachment
        {
            typedef _RenderPassAttachmentCATs<CATs...> ColorAttachments;

            template<typename U = DepthAttachmentType>
            RenderPassAttachment(const ColorAttachments& colorAttachments, typename std::enable_if<std::is_same<U, void>::value>::type* = 0) :
                _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthAttachment<void>()
            {

            }

            template<typename U = DepthAttachmentType>
            RenderPassAttachment(const ColorAttachments& colorAttachments, AttachmentDesc<DepthAttachmentType>& depthAttachment, typename std::enable_if<!std::is_same<U, void>::value>::type* = 0):
            _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthAttachment<DepthAttachmentType>(depthAttachment)
            {
                
            }

            static const size_t ms_ColorAttachmentCounts = sizeof...(CATs);
        };
    }
}