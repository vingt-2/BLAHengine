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


        template<typename AttachmentFormat>
        struct AttachmentDesc
        {
            AttachmentDesc(Gpu::Image<AttachmentFormat>* image) : m_image(image) {}

            Gpu::Image<AttachmentFormat>* m_image;
        };

        struct BaseRenderPassAttachment
        {
            BaseRenderPassAttachment(blaU16 colorAttachmentCount) : m_colorCount(colorAttachmentCount) {}

            void GetColorAttachment(blaU32 i, Gpu::BaseImage*& image) const
            {
                //TODO fatal assert i <= m_colorCount

                const struct UntypedAttachmentDesc
                {
                    Gpu::BaseImage* m_image;
                } attachmentDesc = *reinterpret_cast<const UntypedAttachmentDesc*>((blaU8*)(this + 1) + sizeof(UntypedAttachmentDesc) * i);
                image = attachmentDesc.m_image;
            }

            blaU16 m_colorCount;

            Gpu::BaseImage* m_depthImage = nullptr;
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

        template<typename _DepthAttachmentType, typename... CATs>
        struct RenderPassAttachment<_RenderPassAttachmentTemplateHelpers::CATS<CATs...>, _DepthAttachmentType> : BaseRenderPassAttachment,
            _RenderPassAttachmentCATs<CATs...>, _DepthAttachment<_DepthAttachmentType>
        {
            typedef _RenderPassAttachmentCATs<CATs...> Color;
            typedef _DepthAttachmentType Depth;

            template<typename U = _DepthAttachmentType>
            RenderPassAttachment(const Color& colorAttachments, typename std::enable_if<std::is_same<U, void>::value>::type* = 0) : BaseRenderPassAttachment(ms_colorAttachmentCounts), 
                _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthAttachment<void>()
            {}

            template<typename U = _DepthAttachmentType>
            RenderPassAttachment(const Color& colorAttachments, const AttachmentDesc<_DepthAttachmentType>& depthAttachment, typename std::enable_if<!std::is_same<U, void>::value>::type* = 0):
                BaseRenderPassAttachment(ms_colorAttachmentCounts), _RenderPassAttachmentCATs<CATs...>(colorAttachments), _DepthAttachment<_DepthAttachmentType>(depthAttachment)
            {
                m_depthImage = depthAttachment.m_image;
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