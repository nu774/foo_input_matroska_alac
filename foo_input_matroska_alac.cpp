#include <cstring>
#include <vector>
#include <iterator>
#include "../SDK/foobar2000.h"

class packet_decoder_matroska_alac_bridge: public packet_decoder {
    service_ptr_t<packet_decoder> m_decoder;
public:
    static bool g_is_our_setup(const GUID &p_owner,  t_size p_param1,
                               const void *p_param2, t_size p_param2size)
    {
        auto setup = static_cast<const matroska_setup *>(p_param2);
        return p_owner == owner_matroska
            && p_param2size == sizeof(matroska_setup)
            && std::strncmp(setup->codec_id, "A_ALAC", 6) == 0;
    }
    void open(const GUID &p_owner, bool p_decode, t_size p_param1,
              const void *p_param2, t_size p_param2size,
              abort_callback &p_abort)
    {
        auto setup = static_cast<const matroska_setup *>(p_param2);
        std::vector<char> cookie(4); /* 4 bytes room for version and flags */
        auto p = static_cast<const char*>(setup->codec_private);
        std::copy(p, p + 24, std::back_inserter(cookie));
        packet_decoder::g_open(m_decoder, p_decode, owner_MP4_ALAC, 0,
                               cookie.data(), cookie.size(), p_abort);
    }
    t_size set_stream_property(const GUID &p_type, t_size p_param1,
                               const void *p_param2, t_size p_param2size)
    {
        return m_decoder->set_stream_property(p_type, p_param1, p_param2,
                                              p_param2size);
    }
    void get_info(file_info &p_info)
    {
        m_decoder->get_info(p_info);
    }
    unsigned get_max_frame_dependency()
    {
        return m_decoder->get_max_frame_dependency();
    }
    double get_max_frame_dependency_time()
    {
        return m_decoder->get_max_frame_dependency_time();
    }
    void reset_after_seek()
    {
        m_decoder->reset_after_seek();
    }
    virtual void decode(const void *p_buffer, t_size p_bytes,
                        audio_chunk &p_chunk, abort_callback &p_abort)
    {
        m_decoder->decode(p_buffer, p_bytes, p_chunk, p_abort);
    }
    bool analyze_first_frame_supported()
    {
        return m_decoder->analyze_first_frame_supported();
    }
    virtual void analyze_first_frame(const void *p_buffer, t_size p_bytes,
                                     abort_callback &p_abort)
    {
        m_decoder->analyze_first_frame(p_buffer, p_bytes, p_abort);
    }
};

static packet_decoder_factory_t<packet_decoder_matroska_alac_bridge>
    g_packet_decoder_matroska_alac_bridge_factory;

DECLARE_COMPONENT_VERSION(
    "foo_input_matroska_alac",
    "0.0.1",
    "Matroska/ALAC Bridge"
);
VALIDATE_COMPONENT_FILENAME("foo_input_matroska_alac.dll");
