#include "duilib/RenderSkia/SkiaTextData.h"
#include "duilib/RenderSkia/SkUtils.h"
#include "duilib/Utils/StringConvert.h"

namespace ui
{

SkiaTextData::SkiaTextData()
    : m_text(nullptr)
    , m_byteLength(0)
    , m_textEncoding(SkTextEncoding::kUTF8)
{
}

SkiaTextData::SkiaTextData(const void* text, size_t byteLength, SkTextEncoding textEncoding)
    : m_text(text)
    , m_byteLength(byteLength)
    , m_textEncoding(textEncoding)
{
}

void SkiaTextData::SetText(const void* text, size_t byteLength, SkTextEncoding textEncoding)
{
    m_text = text;
    m_byteLength = byteLength;
    m_textEncoding = textEncoding;
#ifdef _DEBUG
    if (byteLength > 0) {
        switch (textEncoding) {
        case SkTextEncoding::kUTF8:
            SkASSERT(SkUTF8_CountUnichars((const char*)pText, nByteLen) != -1);
            break;
        case SkTextEncoding::kUTF16:
            SkASSERT(SkUTF16_CountUnichars((const char*)pText, nByteLen) != -1);
            break;
        case SkTextEncoding::kUTF32:
            SkASSERT(SkUTF32_CountUnichars((const char*)pText, nByteLen) != -1);
            break;
        default:
            SkASSERT(false);
            break;
        }
    }
#endif
}

bool SkiaTextData::IsEmpty() const
{
    if ((m_text == nullptr) || (m_byteLength == 0)) {
        return true;
    }
    switch (m_textEncoding) {
    case SkTextEncoding::kUTF8:
    case SkTextEncoding::kUTF16:
    case SkTextEncoding::kUTF32:
        return false;
    default:
        SkASSERT(false);
        break;
    }
    return true;
}

size_t SkiaTextData::GetCharByteLength(const void* text, SkTextEncoding textEncoding)
{
    if (textEncoding == SkTextEncoding::kUTF8) {
        int32_t type = SkUTF8_ByteType(*(const uint8_t*)text);
        if (type == 0) return 1;
        if (type == 1) return 2;
        if (type == 2) return 3;
        if (type == 3) return 4;
        return 1;
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        uint16_t c = *(const uint16_t*)text;
        if (SkUTF16_IsHighSurrogate(c)) {
            return 4;
        }
        return 2;
    }
    return 4;
}

bool SkiaTextData::EnumChars(EnumTextCallback callback) const
{
    const void* text = m_text;
    size_t byteLength = m_byteLength;
    SkTextEncoding textEncoding = m_textEncoding;
    if ((text == nullptr) || (byteLength == 0)) {
        return true;
    }
    if (textEncoding == SkTextEncoding::kUTF8) {
        const uint8_t* utf8 = static_cast<const uint8_t*>(text);
        const uint8_t* stop = utf8 + byteLength;
        int32_t type = 0;
        SkUnichar unichar = 0;
        size_t charByteLen = 1;
        while (utf8 < stop) {
            type = SkUTF8_ByteType(*utf8);
            if (!SkUTF8_TypeIsValidLeadingByte(type) || utf8 + type > stop) {
                return false;
            }
            unichar = 0;
            charByteLen = 1;
            if (type == 0) {
                unichar = *utf8;
            }
            else if (type == 1) {
                unichar = ((utf8[0] & 0x1F) << 6) | (utf8[1] & 0x3F);
                charByteLen = 2;
            }
            else if (type == 2) {
                unichar = ((utf8[0] & 0x0F) << 12) | ((utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
                charByteLen = 3;
            }
            else if (type == 3) {
                unichar = ((utf8[0] & 0x07) << 18) | ((utf8[1] & 0x3F) << 12) | ((utf8[2] & 0x3F) << 6) | (utf8[3] & 0x3F);
                charByteLen = 4;
            }
            else if (type == 4) {
                return false;
            }
            if (!callback(unichar, charByteLen)) {
                return true;
            }
            utf8 += charByteLen;
        }
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        const uint16_t* utf16 = static_cast<const uint16_t*>(text);
        const uint16_t* stop = utf16 + (byteLength >> 1);
        uint16_t c = 0;
        size_t charByteLen = 0;
        SkUnichar unichar = 0;
        uint16_t low = 0;
        constexpr SkUnichar unicharLowOffset = (0x10000 - (0xD800 << 10) - 0xDC00);
        while (utf16 < stop) {
            c = *utf16++;
            charByteLen = sizeof(uint16_t);
            unichar = c;
            if (SkUTF16_IsHighSurrogate(c)) {
                if (utf16 >= stop) {
                    return true;
                }
                low = *utf16++;
                if (!SkUTF16_IsLowSurrogate(low)) {
                    return false;
                }
                unichar = (c << 10) + low + unicharLowOffset;
                charByteLen = sizeof(uint32_t);
            }
            if (!callback(unichar, charByteLen)) {
                return true;
            }
        }
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        SkASSERT(byteLength % sizeof(SkUnichar) == 0);
        const SkUnichar* utf32 = static_cast<const SkUnichar*>(text);
        const SkUnichar* stop = utf32 + (byteLength / sizeof(SkUnichar));
        while (utf32 < stop) {
            if (!callback(*utf32, sizeof(SkUnichar))) {
                return true;
            }
            ++utf32;
        }
    }
    else {
        SkASSERT(false);
        return false;
    }
    return true;
}

UTF32String SkiaTextData::ToUTF32String() const
{
    if (IsEmpty()) {
        return UTF32String();
    }
    if (m_textEncoding == SkTextEncoding::kUTF8) {
        return StringConvert::UTF8ToUTF32((const DUTF8Char*)m_text, m_byteLength / sizeof(DUTF8Char));
    }
    else if (m_textEncoding == SkTextEncoding::kUTF16) {
        ASSERT(m_byteLength % sizeof(DUTF16Char) == 0);
        return StringConvert::UTF16ToUTF32((const DUTF16Char*)m_text, m_byteLength / sizeof(DUTF16Char));
    }
    else if (m_textEncoding == SkTextEncoding::kUTF32) {
        ASSERT(m_byteLength % sizeof(DUTF32Char) == 0);
        return UTF32String((const DUTF32Char*)m_text, m_byteLength / sizeof(DUTF32Char));
    }
    ASSERT(0);
    return UTF32String();
}

} // namespace ui
