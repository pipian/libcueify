%module cueify
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
#pragma SWIG nowarn=SWIGWARN_PARSE_NESTED_CLASS

%{
#include <cueify/cueify.hxx>
#include <cueify/error.h>
#include <cueify/constants.h>
%}

%{
namespace cueify {
typedef TOC::Track TOCTrack;
typedef FullTOC::Track FullTOCTrack;
typedef FullTOC::Session::Pseudotrack FullTOCSessionPseudotrack;
typedef FullTOC::Session FullTOCSession;
typedef CDText::TOC::Track::Interval CDTextTOCTrackInterval;
typedef CDText::TOC::Track CDTextTOCTrack;
typedef CDText::TOC CDTextTOC;
typedef CDText::Block::Track CDTextBlockTrack;
typedef CDText::Block CDTextBlock;
};
%}

namespace std {
    %template(VectorTOCTrack) vector<cueify::TOCTrack>;
    %template(VectorFullTOCTrack) vector<cueify::FullTOCTrack>;
    %template(VectorFullTOCSessionPseudotrack) vector<cueify::FullTOCSessionPseudotrack>;
    %template(VectorFullTOCSession) vector<cueify::FullTOCSession>;
    %template(VectorCDTextTOCTrackInterval) vector<cueify::CDTextTOCTrackInterval>;
    %template(VectorCDTextTOCTrack) vector<cueify::CDTextTOCTrack>;
    %template(VectorCDTextBlockTrack) vector<cueify::CDTextBlockTrack>;
    %template(VectorCDTextBlock) vector<cueify::CDTextBlock>;
    %template(VectorTrackIndices) vector<cueify::TrackIndex>;
};

#if defined(SWIGRUBY)
%rename(supported_apis) supportedAPIs;
%rename(error_code) errorCode;
#endif

%extend cueify::Device {
    const int supportedAPIs;
    const int errorCode;
}
%ignore cueify::Device::supportedAPIs;
%ignore cueify::Device::errorCode;
%newobject cueify::Device::readTOC;
%newobject cueify::Device::readSessions;
%newobject cueify::Device::readFullTOC;
%newobject cueify::Device::readCDText;

#if defined(SWIGRUBY)
%rename(control_flags) controlFlags;
%rename(subqchannel_format) subQChannelFormat;
#endif

namespace cueify {
class TOCTrack {
public:
    %extend {
	const uint8_t controlFlags;
	const uint8_t subQChannelFormat;
	const uint32_t address;
	const uint32_t length;
    }
};
};

#if defined(SWIGRUBY)
%rename(error_code) errorCode;
%rename(first_track) firstTrack;
%rename(last_track) lastTrack;
%rename(leadout_track) leadoutTrack;
%rename(disc_length) discLength;
#endif

%extend cueify::TOC {
    const int errorCode;
    const uint8_t firstTrack;
    const uint8_t lastTrack;
    const std::vector<cueify::TOCTrack> tracks;
    const cueify::TOCTrack leadoutTrack;
    const uint32_t discLength;
}
%ignore cueify::TOC::errorCode;
%ignore cueify::TOC::firstTrack;
%ignore cueify::TOC::lastTrack;
%ignore cueify::TOC::tracks;
%ignore cueify::TOC::leadoutTrack;
%ignore cueify::TOC::discLength;

#if defined(SWIGRUBY)
%rename(error_code) errorCode;
%rename(first_session) firstSession;
%rename(last_session) lastSession;
%rename(last_session_control_flags) lastSessionControlFlags;
%rename(last_session_subqchannel_format) lastSessionSubQChannelFormat;
%rename(last_session_track_number) lastSessionTrackNumber;
%rename(last_session_address) lastSessionAddress;
#endif

%extend cueify::Sessions {
    const int errorCode;
    const uint8_t firstSession;
    const uint8_t lastSession;
    const uint8_t lastSessionControlFlags;
    const uint8_t lastSessionSubQChannelFormat;
    const uint8_t lastSessionTrackNumber;
    const uint32_t lastSessionAddress;
}
%ignore cueify::Sessions::errorCode;
%ignore cueify::Sessions::firstSession;
%ignore cueify::Sessions::lastSession;
%ignore cueify::Sessions::lastSessionControlFlags;
%ignore cueify::Sessions::lastSessionSubQChannelFormat;
%ignore cueify::Sessions::lastSessionTrackNumber;
%ignore cueify::Sessions::lastSessionAddress;

#if defined(SWIGRUBY)
%rename(control_flags) controlFlags;
%rename(subqchannel_format) subQChannelFormat;
%rename(point_address) pointAddress;
#endif

namespace cueify {
class FullTOCTrack {
public:
    %extend {
	const uint8_t session;
	const uint8_t controlFlags;
	const uint8_t subQChannelFormat;
	const cueify::MSFAddress pointAddress;
	const cueify::MSFAddress address;
	const cueify::MSFAddress length;
    }
};
};

#if defined(SWIGRUBY)
%rename(control_flags) controlFlags;
%rename(subqchannel_format) subQChannelFormat;
%rename(point_address) pointAddress;
#endif

namespace cueify {
class FullTOCSessionPseudotrack {
public:
    %extend {
	const uint8_t controlFlags;
	const uint8_t subQChannelFormat;
	const cueify::MSFAddress pointAddress;
    }
};
};

#if defined(SWIGRUBY)
%rename(first_track) firstTrack;
%rename(last_track) lastTrack;
%rename(leadout_address) leadoutAddress;
#endif

namespace cueify {
class FullTOCSession {
public:
    %extend {
	const std::vector<cueify::FullTOCSessionPseudotrack> pseudotracks;
	const uint8_t firstTrack;
	const uint8_t lastTrack;
	const uint8_t type;
	const cueify::MSFAddress leadoutAddress;
	const cueify::MSFAddress length;
    }
};
};

#if defined(SWIGRUBY)
%rename(error_code) errorCode;
%rename(first_session) firstSession;
%rename(last_session) lastSession;
%rename(first_track) firstTrack;
%rename(last_track) lastTrack;
%rename(disc_length) discLength;
#endif

%extend cueify::FullTOC {
    const int errorCode;
    const uint8_t firstSession;
    const uint8_t lastSession;
    const std::vector<cueify::FullTOCTrack> tracks;
    const std::vector<cueify::FullTOCSession> sessions;
    const uint8_t firstTrack;
    const uint8_t lastTrack;
    const cueify::MSFAddress discLength;
}
%ignore cueify::FullTOC::errorCode;
%ignore cueify::FullTOC::firstSession;
%ignore cueify::FullTOC::lastSession;
%ignore cueify::FullTOC::tracks;
%ignore cueify::FullTOC::sessions;
%ignore cueify::FullTOC::firstTrack;
%ignore cueify::FullTOC::lastTrack;
%ignore cueify::FullTOC::discLength;

namespace cueify {
class CDTextTOCTrackInterval {
public:
    %extend {
	const cueify::MSFAddress start;
	const cueify::MSFAddress end;
    }
};
};

namespace cueify {
class CDTextTOCTrack {
public:
    %extend {
	const std::vector<cueify::CDTextTOCTrackInterval> intervals;
	const cueify::MSFAddress offset;
    }
};
};

#if defined(SWIGRUBY)
%rename(first_track) firstTrack;
%rename(last_track) lastTrack;
#endif

namespace cueify {
class CDTextTOC {
public:
    %extend {
	const uint8_t firstTrack;
	const uint8_t lastTrack;
	const std::vector<cueify::CDTextTOCTrack> tracks;
    }
};
};

#if defined(SWIGRUBY)
%rename("has_title?") hasTitle;
%rename("has_performer?") hasPerformer;
%rename("has_songwriter?") hasSongwriter;
%rename("has_composer?") hasComposer;
%rename("has_arranger?") hasArranger;
%rename("has_message?") hasMessage;
%rename("has_private_data?") hasPrivateData;
%rename(private_data) privateData;
%rename("has_upc_isrc?") hasUPCISRC;
%rename(upc_isrc) upcISRC;
#endif

/* TODO: Return nil for title, etc. in Ruby */
/* TODO: Automatically return unicode strings in languages which support such a type */
/* TODO: Write a plethora of tests */
namespace cueify {
class CDTextBlockTrack {
public:
    %extend {
	const bool hasTitle;
	const std::string title;
	const bool hasPerformer;
	const std::string performer;
	const bool hasSongwriter;
	const std::string songwriter;
	const bool hasComposer;
	const std::string composer;
	const bool hasArranger;
	const std::string arranger;
	const bool hasMessage;
	const std::string message;
	const bool hasPrivateData;
	const std::string privateData;
	const bool hasUPCISRC;
	const std::string upcISRC;
    }
};
};

#if defined(SWIGPYTHON)
%pythoncode %{
CDTextBlockTrack.__swig_getmethods__["title"] = lambda self: None if not self.hasTitle else _cueify.CDTextBlockTrack_title_get(self)
if _newclass:CDTextBlockTrack.title = _swig_property(lambda self: None if not self.hasTitle else _cueify.CDTextBlockTrack_title_get(self))
CDTextBlockTrack.__swig_getmethods__["performer"] = lambda self: None if not self.hasPerformer else _cueify.CDTextBlockTrack_performer_get(self)
if _newclass:CDTextBlockTrack.performer = _swig_property(lambda self: None if not self.hasPerformer else _cueify.CDTextBlockTrack_performer_get(self))
CDTextBlockTrack.__swig_getmethods__["songwriter"] = lambda self: None if not self.hasSongwriter else _cueify.CDTextBlockTrack_songwriter_get(self)
if _newclass:CDTextBlockTrack.songwriter = _swig_property(lambda self: None if not self.hasSongwriter else _cueify.CDTextBlockTrack_songwriter_get(self))
CDTextBlockTrack.__swig_getmethods__["composer"] = lambda self: None if not self.hasComposer else _cueify.CDTextBlockTrack_composer_get(self)
if _newclass:CDTextBlockTrack.composer = _swig_property(lambda self: None if not self.hasComposer else _cueify.CDTextBlockTrack_composer_get(self))
CDTextBlockTrack.__swig_getmethods__["arranger"] = lambda self: None if not self.hasArranger else _cueify.CDTextBlockTrack_arranger_get(self)
if _newclass:CDTextBlockTrack.arranger = _swig_property(lambda self: None if not self.hasArranger else _cueify.CDTextBlockTrack_arranger_get(self))
CDTextBlockTrack.__swig_getmethods__["message"] = lambda self: None if not self.hasMessage else _cueify.CDTextBlockTrack_message_get(self)
if _newclass:CDTextBlockTrack.message = _swig_property(lambda self: None if not self.hasMessage else _cueify.CDTextBlockTrack_message_get(self))
CDTextBlockTrack.__swig_getmethods__["privateData"] = lambda self: None if not self.hasPrivateData else _cueify.CDTextBlockTrack_privateData_get(self)
if _newclass:CDTextBlockTrack.privateData = _swig_property(lambda self: None if not self.hasPrivateData else _cueify.CDTextBlockTrack_privateData_get(self))
CDTextBlockTrack.__swig_getmethods__["upcISRC"] = lambda self: None if not self.hasUPCISRC else _cueify.CDTextBlockTrack_upcISRC_get(self)
if _newclass:CDTextBlockTrack.upcISRC = _swig_property(lambda self: None if not self.hasUPCISRC else _cueify.CDTextBlockTrack_upcISRC_get(self))
%}
#endif

#if defined(SWIGRUBY)
%rename(first_track) firstTrack;
%rename(last_track) lastTrack;
%rename("has_program_data?") hasProgramData;
%rename("has_program_copyright?") hasProgramCopyright;
%rename("has_message_copyright?") hasMessageCopyright;
%rename("has_name_copyright?") hasNameCopyright;
%rename("has_title_copyright?") hasTitleCopyright;
%rename("has_discid?") hasDiscid;
%rename(genre_code) genreCode;
%rename(genre_name) genreName;
#endif

namespace cueify {
class CDTextBlock {
public:
    %extend {
	const uint8_t firstTrack;
	const uint8_t lastTrack;
	const uint8_t charset;
	const uint8_t language;
	const bool hasProgramData;
	const bool hasProgramCopyright;
	const bool hasMessageCopyright;
	const bool hasNameCopyright;
	const bool hasTitleCopyright;
	const std::vector<cueify::CDTextBlockTrack> tracks;
	const bool hasDiscid;
	const std::string discid;
	const uint16_t genreCode;
	const std::string genreName;
    }
};
};

#if defined(SWIGPYTHON)
%pythoncode %{
CDTextBlock.__swig_getmethods__["discid"] = lambda self: None if not self.hasDiscid else _cueify.CDTextBlock_discid_get(self)
if _newclass:CDTextBlock.discid = _swig_property(lambda self: None if not self.hasDiscid else _cueify.CDTextBlock_discid_get(self))
%}
#endif

#if defined(SWIGRUBY)
%rename(error_code) errorCode;
#endif

%extend cueify::CDText {
    const int errorCode;
    const cueify::CDTextTOC toc;
    const std::vector<cueify::CDTextBlock> blocks;
}
%ignore cueify::CDText::errorCode;
%ignore cueify::CDText::toc;
%ignore cueify::CDText::blocks;

%extend cueify::TrackIndex {
    const uint8_t number;
    const MSFAddress offset;
}
%ignore cueify::TrackIndex::number;
%ignore cueify::TrackIndex::offset;

%include <cueify/cueify.hxx>

/* Imported from constants.h */
#define LEAD_OUT_TRACK  0xAA
#define TOC_TRACK_HAS_PREEMPHASIS  0x1
#define TOC_TRACK_PERMITS_COPYING  0x2
#define TOC_TRACK_IS_DATA          0x4
#define TOC_TRACK_IS_QUADRAPHONIC  0x8
#define SUB_Q_NOTHING   0x0
#define SUB_Q_POSITION  0x1
#define SUB_Q_MCN       0x2
#define SUB_Q_ISRC      0x3

/* Imported from error.h */
%inline %{ enum {
    OK = CUEIFY_OK,
#if defined(SWIGPERL)
    NODATA,
#else
    NO_DATA,
#endif
    ERR_INTERNAL,
    ERR_BADARG,
    ERR_NOMEM,
    ERR_NO_DEVICE,
    ERR_TRUNCATED,
    ERR_CORRUPTED,
    ERR_TOOSMALL,
    ERR_INVALID_CDTEXT
};
%}

/* Imported (with modification) from full_toc.h */
#define FULL_TOC_FIRST_TRACK_PSEUDOTRACK  0
#define FULL_TOC_LAST_TRACK_PSEUDOTRACK   1
#define FULL_TOC_LEAD_OUT_TRACK           2

#define SESSION_MODE_1  0x00
#define SESSION_CDI     0x10
#define SESSION_MODE_2  0x20

/* Imported from cdtext.h */
#define CDTEXT_CHARSET_ISO8859_1  0x00
#define CDTEXT_CHARSET_ASCII      0x01
#define CDTEXT_CHARSET_MSJIS      0x80
#define CDTEXT_CHARSET_KOREAN     0x81
#define CDTEXT_CHARSET_CHINESE    0x82

%inline %{ enum {
    CDTEXT_LANG_UNKNOWN  = 0x00,
    CDTEXT_LANG_ALBANIAN,
    CDTEXT_LANG_BRETON,
    CDTEXT_LANG_CATALAN,
    CDTEXT_LANG_CROATIAN,
    CDTEXT_LANG_WELSH,
    CDTEXT_LANG_CZECH,
    CDTEXT_LANG_DANISH,
    CDTEXT_LANG_GERMAN,
    CDTEXT_LANG_ENGLISH,
    CDTEXT_LANG_SPANISH,
    CDTEXT_LANG_ESPERANTO,
    CDTEXT_LANG_ESTONIAN,
    CDTEXT_LANG_BASQUE,
    CDTEXT_LANG_FAROESE,
    CDTEXT_LANG_FRENCH,
    CDTEXT_LANG_FRISIAN,
    CDTEXT_LANG_IRISH,
    CDTEXT_LANG_GAELIC,
    CDTEXT_LANG_GALICIAN,
    CDTEXT_LANG_ICELANDIC,
    CDTEXT_LANG_ITALIAN,
    CDTEXT_LANG_SAMI,
    CDTEXT_LANG_LATIN,
    CDTEXT_LANG_LATVIAN,
    CDTEXT_LANG_LUXEMBOURGISH,
    CDTEXT_LANG_LITHUANIAN,
    CDTEXT_LANG_HUNGARIAN,
    CDTEXT_LANG_MALTESE,
    CDTEXT_LANG_DUTCH,
    CDTEXT_LANG_NORWEGIAN,
    CDTEXT_LANG_OCCITAN,
    CDTEXT_LANG_POLISH,
    CDTEXT_LANG_PORTUGUESE,
    CDTEXT_LANG_ROMANIAN,
    CDTEXT_LANG_ROMANSH,
    CDTEXT_LANG_SERBIAN,
    CDTEXT_LANG_SLOVAK,
    CDTEXT_LANG_SLOVENIAN,
    CDTEXT_LANG_FINNISH,
    CDTEXT_LANG_SWEDISH,
    CDTEXT_LANG_TURKISH,
    CDTEXT_LANG_FLEMISH,
    CDTEXT_LANG_WALLOON,
    CDTEXT_LANG_ZULU = 0x45,
    CDTEXT_LANG_VIETNAMESE,
    CDTEXT_LANG_UZBEK,
    CDTEXT_LANG_URDU,
    CDTEXT_LANG_UKRAINIAN,
    CDTEXT_LANG_THAI,
    CDTEXT_LANG_TELUGU,
    CDTEXT_LANG_TATAR,
    CDTEXT_LANG_TAMIL,
    CDTEXT_LANG_TAJIK,
    CDTEXT_LANG_SWAHILI,
    CDTEXT_LANG_SRANAN_TONGO,
    CDTEXT_LANG_SOMALI,
    CDTEXT_LANG_SINHALA,
    CDTEXT_LANG_SHONA,
    CDTEXT_LANG_SERBOCROAT,
    CDTEXT_LANG_RUTHENIAN,
    CDTEXT_LANG_RUSSIAN,
    CDTEXT_LANG_QUECHUA,
    CDTEXT_LANG_PUSHTU,
    CDTEXT_LANG_PUNJABI,
    CDTEXT_LANG_PERSIAN,
    CDTEXT_LANG_PAPIAMENTO,
    CDTEXT_LANG_ORIYA,
    CDTEXT_LANG_NEPALI,
    CDTEXT_LANG_NDEBELE,
    CDTEXT_LANG_MARATHI,
    CDTEXT_LANG_MOLDAVIAN,
    CDTEXT_LANG_MALAYSIAN,
    CDTEXT_LANG_MALAGASY,
    CDTEXT_LANG_MACEDONIAN,
    CDTEXT_LANG_LAO,
    CDTEXT_LANG_KOREAN,
    CDTEXT_LANG_KHMER,
    CDTEXT_LANG_KAZAKH,
    CDTEXT_LANG_KANNADA,
    CDTEXT_LANG_JAPANESE,
    CDTEXT_LANG_INDONESIAN,
    CDTEXT_LANG_HINDI,
    CDTEXT_LANG_HEBREW,
    CDTEXT_LANG_HAUSA,
    CDTEXT_LANG_GUARANI,
    CDTEXT_LANG_GUJARATI,
    CDTEXT_LANG_GREEK,
    CDTEXT_LANG_GEORGIAN,
    CDTEXT_LANG_FULAH,
    CDTEXT_LANG_DARI,
    CDTEXT_LANG_CHUVASH,
    CDTEXT_LANG_CHINESE,
    CDTEXT_LANG_BURMESE,
    CDTEXT_LANG_BULGARIAN,
    CDTEXT_LANG_BENGALI,
    CDTEXT_LANG_BELARUSIAN,
    CDTEXT_LANG_BAMBARA,
    CDTEXT_LANG_AZERBAIJANI,
    CDTEXT_LANG_ASSAMESE,
    CDTEXT_LANG_ARMENIAN,
    CDTEXT_LANG_ARABIC,
    CDTEXT_LANG_AMHARIC
};
%}

#define CDTEXT_ALBUM  0x00

/* Imported from track_data.h */
#define DATA_MODE_CDDA     0x00
#define DATA_MODE_MODE_1   0x01
#define DATA_MODE_MODE_2   0x02
#define DATA_MODE_UNKNOWN  0x0F
#define DATA_MODE_ERROR    0x10


%{
int cueify_Device_supportedAPIs_get(cueify::Device *d) {
    return d->supportedAPIs();
}

int cueify_Device_errorCode_get(cueify::Device *d) {
    return d->errorCode();
}

int cueify_TOC_errorCode_get(cueify::TOC *t) {
    return t->errorCode();
}

uint8_t cueify_TOC_firstTrack_get(cueify::TOC *t) {
    return t->firstTrack();
}

uint8_t cueify_TOC_lastTrack_get(cueify::TOC *t) {
    return t->lastTrack();
}

std::vector<cueify::TOCTrack>* cueify_TOC_tracks_get(cueify::TOC *t) {
    return new std::vector<cueify::TOCTrack>(t->tracks());
}

cueify::TOCTrack* cueify_TOC_leadoutTrack_get(cueify::TOC *t) {
    return new cueify::TOCTrack(t->leadoutTrack());
}

uint8_t cueify_TOCTrack_controlFlags_get(cueify::TOCTrack *t) {
    return t->controlFlags();
}

uint8_t cueify_TOCTrack_subQChannelFormat_get(cueify::TOCTrack *t) {
    return t->subQChannelFormat();
}

uint32_t cueify_TOCTrack_address_get(cueify::TOCTrack *t) {
    return t->address();
}

uint32_t cueify_TOCTrack_length_get(cueify::TOCTrack *t) {
    return t->length();
}

uint32_t cueify_TOC_discLength_get(cueify::TOC *t) {
    return t->discLength();
}

int cueify_Sessions_errorCode_get(cueify::Sessions *s) {
    return s->errorCode();
}

uint8_t cueify_Sessions_firstSession_get(cueify::Sessions *s) {
    return s->firstSession();
}

uint8_t cueify_Sessions_lastSession_get(cueify::Sessions *s) {
    return s->lastSession();
}

uint8_t cueify_Sessions_lastSessionControlFlags_get(cueify::Sessions *s) {
    return s->lastSessionControlFlags();
}

uint8_t cueify_Sessions_lastSessionSubQChannelFormat_get(cueify::Sessions *s) {
    return s->lastSessionSubQChannelFormat();
}

uint8_t cueify_Sessions_lastSessionTrackNumber_get(cueify::Sessions *s) {
    return s->lastSessionTrackNumber();
}

uint32_t cueify_Sessions_lastSessionAddress_get(cueify::Sessions *s) {
    return s->lastSessionAddress();
}

uint8_t cueify_FullTOCTrack_session_get(cueify::FullTOCTrack *t) {
    return t->session();
}

uint8_t cueify_FullTOCTrack_controlFlags_get(cueify::FullTOCTrack *t) {
    return t->controlFlags();
}

uint8_t cueify_FullTOCTrack_subQChannelFormat_get(cueify::FullTOCTrack *t) {
    return t->subQChannelFormat();
}

cueify::MSFAddress* cueify_FullTOCTrack_address_get(cueify::FullTOCTrack *t) {
    return new cueify::MSFAddress(t->address());
}

cueify::MSFAddress* cueify_FullTOCTrack_pointAddress_get(
    cueify::FullTOCTrack *t) {
    return new cueify::MSFAddress(t->pointAddress());
}

cueify::MSFAddress* cueify_FullTOCTrack_length_get(cueify::FullTOCTrack *t) {
    return new cueify::MSFAddress(t->length());
}

uint8_t cueify_FullTOCSessionPseudotrack_controlFlags_get(
    cueify::FullTOCSessionPseudotrack *t) {
    return t->controlFlags();
}

uint8_t cueify_FullTOCSessionPseudotrack_subQChannelFormat_get(
    cueify::FullTOCSessionPseudotrack *t) {
    return t->subQChannelFormat();
}

cueify::MSFAddress* cueify_FullTOCSessionPseudotrack_pointAddress_get(
    cueify::FullTOCSessionPseudotrack *t) {
    return new cueify::MSFAddress(t->pointAddress());
}

std::vector<cueify::FullTOCSessionPseudotrack>*
    cueify_FullTOCSession_pseudotracks_get(
	cueify::FullTOCSession *s) {
    return new std::vector<cueify::FullTOCSessionPseudotrack>(
	s->pseudotracks());
}

uint8_t cueify_FullTOCSession_firstTrack_get(cueify::FullTOCSession *s) {
    return s->firstTrack();
}

uint8_t cueify_FullTOCSession_lastTrack_get(cueify::FullTOCSession *s) {
    return s->lastTrack();
}

uint8_t cueify_FullTOCSession_type_get(cueify::FullTOCSession *s) {
    return s->type();
}

cueify::MSFAddress* cueify_FullTOCSession_leadoutAddress_get(
    cueify::FullTOCSession *s) {
    return new cueify::MSFAddress(s->leadoutAddress());
}

cueify::MSFAddress* cueify_FullTOCSession_length_get(
    cueify::FullTOCSession *s) {
    return new cueify::MSFAddress(s->length());
}

int cueify_FullTOC_errorCode_get(cueify::FullTOC *t) {
    return t->errorCode();
}

uint8_t cueify_FullTOC_firstSession_get(cueify::FullTOC *t) {
    return t->firstSession();
}

uint8_t cueify_FullTOC_lastSession_get(cueify::FullTOC *t) {
    return t->lastSession();
}

std::vector<cueify::FullTOCSession>* cueify_FullTOC_sessions_get(
    cueify::FullTOC *t) {
    return new std::vector<cueify::FullTOCSession>(t->sessions());
}

std::vector<cueify::FullTOCTrack>* cueify_FullTOC_tracks_get(
    cueify::FullTOC *t) {
    return new std::vector<cueify::FullTOCTrack>(t->tracks());
}

uint8_t cueify_FullTOC_firstTrack_get(cueify::FullTOC *t) {
    return t->firstTrack();
}

uint8_t cueify_FullTOC_lastTrack_get(cueify::FullTOC *t) {
    return t->lastTrack();
}

cueify::MSFAddress* cueify_FullTOC_discLength_get(cueify::FullTOC *t) {
    return new cueify::MSFAddress(t->discLength());
}

cueify::MSFAddress* cueify_CDTextTOCTrackInterval_start_get(
    cueify::CDTextTOCTrackInterval *i) {
    return new cueify::MSFAddress(i->start());
}

cueify::MSFAddress* cueify_CDTextTOCTrackInterval_end_get(
    cueify::CDTextTOCTrackInterval *i) {
    return new cueify::MSFAddress(i->end());
}

std::vector<cueify::CDTextTOCTrackInterval>*
    cueify_CDTextTOCTrack_intervals_get(
    cueify::CDTextTOCTrack *t) {
    return new std::vector<cueify::CDTextTOCTrackInterval>(t->intervals());
}

cueify::MSFAddress* cueify_CDTextTOCTrack_offset_get(
    cueify::CDTextTOCTrack *t) {
    return new cueify::MSFAddress(t->offset());
}

uint8_t cueify_CDTextTOC_firstTrack_get(cueify::CDTextTOC *t) {
    return t->firstTrack();
}

uint8_t cueify_CDTextTOC_lastTrack_get(cueify::CDTextTOC *t) {
    return t->lastTrack();
}

std::vector<cueify::CDTextTOCTrack>* cueify_CDTextTOC_tracks_get(
    cueify::CDTextTOC *t) {
    return new std::vector<cueify::CDTextTOCTrack>(t->tracks());
}

bool cueify_CDTextBlockTrack_hasTitle_get(cueify::CDTextBlockTrack *t) {
    return t->hasTitle();
}

const std::string& cueify_CDTextBlockTrack_title_get(
    cueify::CDTextBlockTrack *t) {
    return t->title();
}

bool cueify_CDTextBlockTrack_hasPerformer_get(cueify::CDTextBlockTrack *t) {
    return t->hasPerformer();
}

const std::string& cueify_CDTextBlockTrack_performer_get(
    cueify::CDTextBlockTrack *t) {
    return t->performer();
}

bool cueify_CDTextBlockTrack_hasSongwriter_get(cueify::CDTextBlockTrack *t) {
    return t->hasSongwriter();
}

const std::string& cueify_CDTextBlockTrack_songwriter_get(
    cueify::CDTextBlockTrack *t) {
    return t->songwriter();
}

bool cueify_CDTextBlockTrack_hasComposer_get(cueify::CDTextBlockTrack *t) {
    return t->hasComposer();
}

const std::string& cueify_CDTextBlockTrack_composer_get(
    cueify::CDTextBlockTrack *t) {
    return t->composer();
}

bool cueify_CDTextBlockTrack_hasArranger_get(cueify::CDTextBlockTrack *t) {
    return t->hasArranger();
}

const std::string& cueify_CDTextBlockTrack_arranger_get(
    cueify::CDTextBlockTrack *t) {
    return t->arranger();
}

bool cueify_CDTextBlockTrack_hasMessage_get(cueify::CDTextBlockTrack *t) {
    return t->hasMessage();
}

const std::string& cueify_CDTextBlockTrack_message_get(
    cueify::CDTextBlockTrack *t) {
    return t->message();
}

bool cueify_CDTextBlockTrack_hasPrivateData_get(cueify::CDTextBlockTrack *t) {
    return t->hasPrivateData();
}

const std::string& cueify_CDTextBlockTrack_privateData_get(
    cueify::CDTextBlockTrack *t) {
    return t->privateData();
}

bool cueify_CDTextBlockTrack_hasUPCISRC_get(cueify::CDTextBlockTrack *t) {
    return t->hasUPCISRC();
}

const std::string& cueify_CDTextBlockTrack_upcISRC_get(
    cueify::CDTextBlockTrack *t) {
    return t->upcISRC();
}

uint8_t cueify_CDTextBlock_firstTrack_get(cueify::CDTextBlock *b) {
    return b->firstTrack();
}

uint8_t cueify_CDTextBlock_lastTrack_get(cueify::CDTextBlock *b) {
    return b->lastTrack();
}

uint8_t cueify_CDTextBlock_charset_get(cueify::CDTextBlock *b) {
    return b->charset();
}

uint8_t cueify_CDTextBlock_language_get(cueify::CDTextBlock *b) {
    return b->language();
}

bool cueify_CDTextBlock_hasProgramData_get(cueify::CDTextBlock *b) {
    return b->hasProgramData();
}

bool cueify_CDTextBlock_hasProgramCopyright_get(cueify::CDTextBlock *b) {
    return b->hasProgramCopyright();
}

bool cueify_CDTextBlock_hasMessageCopyright_get(cueify::CDTextBlock *b) {
    return b->hasMessageCopyright();
}

bool cueify_CDTextBlock_hasNameCopyright_get(cueify::CDTextBlock *b) {
    return b->hasNameCopyright();
}

bool cueify_CDTextBlock_hasTitleCopyright_get(cueify::CDTextBlock *b) {
    return b->hasTitleCopyright();
}

std::vector<cueify::CDTextBlockTrack>* cueify_CDTextBlock_tracks_get(
    cueify::CDTextBlock *b) {
    return new std::vector<cueify::CDTextBlockTrack>(b->tracks());
}

bool cueify_CDTextBlock_hasDiscid_get(cueify::CDTextBlock *b) {
    return b->hasDiscid();
}

const std::string& cueify_CDTextBlock_discid_get(cueify::CDTextBlock *b) {
    return b->discid();
}

uint16_t cueify_CDTextBlock_genreCode_get(cueify::CDTextBlock *b) {
    return b->genreCode();
}

const std::string& cueify_CDTextBlock_genreName_get(cueify::CDTextBlock *b) {
    return b->genreName();
}

uint8_t cueify_CDText_errorCode_get(cueify::CDText *t) {
    return t->errorCode();
}

cueify::CDTextTOC* cueify_CDText_toc_get(cueify::CDText *t) {
    return new cueify::CDTextTOC(t->toc());
}

std::vector<cueify::CDTextBlock>* cueify_CDText_blocks_get(cueify::CDText *t) {
    return new std::vector<cueify::CDTextBlock>(t->blocks());
}

uint8_t cueify_TrackIndex_number_get(cueify::TrackIndex *i) {
    return i->number;
}

cueify::MSFAddress* cueify_TrackIndex_offset_get(cueify::TrackIndex *i) {
    return new cueify::MSFAddress(i->offset);
}
%}
