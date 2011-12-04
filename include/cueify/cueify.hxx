/* cueify.hpp - Header for C++ wrapper of libcueify
 *
 * Copyright (c) 2011 Ian Jacobi <pipian@pipian.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _CUEIFY_CUEIFY_HPP
#define _CUEIFY_CUEIFY_HPP

#include <string>
#include <vector>
#include <cueify/cueify.h>

#define CUEIFY_SERIALIZER(serializer, variable)	\
    uint8_t *buffer = NULL; \
    size_t size = 0; \
    std::string serialization; \
    \
    if ((_errorCode = serializer(variable, buffer, &size)) != CUEIFY_OK) { \
	return std::string(); \
    } \
    buffer = new uint8_t[size]; \
    if (buffer == NULL) { \
	_errorCode = CUEIFY_ERR_NOMEM; \
	return std::string(); \
    } \
    if ((_errorCode = serializer(variable, buffer, &size)) != CUEIFY_OK) { \
	delete buffer; \
	return std::string(); \
    } \
    serialization = std::string(reinterpret_cast<const char *>(buffer), \
				size); \
    delete buffer; \
    return serialization

#define CUEIFY_DESERIALIZER(deserializer, variable) \
    return (_errorCode = deserializer(variable, buffer, size)) == CUEIFY_OK

namespace cueify {

/** A CD-address in the minute-second-frame format. */
class MSFAddress {
public:
    /** The minute value of the address. */
    uint8_t min;
    /** The second value of the address. */
    uint8_t sec;
    /** The frame value of the address. */
    uint8_t frm;

    MSFAddress() : min(0), sec(0), frm(0) { };

    /** Initialize an MSFAddress from a cueify_msf_t struct. */
    MSFAddress(cueify_msf_t msf) : min(msf.min), sec(msf.sec), frm(msf.frm) {};

    /** Initialize an MSFAddress from individual values. */
    MSFAddress(uint8_t min, uint8_t sec, uint8_t frm) :
	min(min), sec(sec), frm(frm) { };
};

/** The table of contents (TOC) of an audio CD. */
class TOC {
protected:
    cueify_toc *_t;
    int _errorCode;
public:
    friend class Device;

    /**
     * Create a new TOC instance. The instance is created with no tracks,
     * and should be populated using TOC::deserialize().
     */
    TOC() : _t(cueify_toc_new()), _errorCode(CUEIFY_OK) { }
    ~TOC() { cueify_toc_free(_t); }

    /**
     * Get the most recent error code from a call to this TOC
     * instance.
     *
     * @return the most recent error code
     */
    int errorCode() const { return _errorCode; };

    /**
     * Deserialize a TOC instance previously serialized with
     * TOC::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0000b, and is compatible with the (poorly defined) definition
     *       of the MCI/MCDI frame in ID3v2.
     *
     * @pre { buffer != NULL }
     * @param buffer a pointer to the serialized TOC data
     * @param size the size of the buffer
     * @return TRUE if the TOC was successfully deserialized
     */
    bool deserialize(const uint8_t * const buffer, size_t size) {
	CUEIFY_DESERIALIZER(cueify_toc_deserialize, _t);
    };  /* TOC::deserialize(const uint8_t * const, size_t) */

    /**
     * Deserialize a TOC instance previously serialized with
     * TOC::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0000b, and is compatible with the (poorly defined) definition
     *       of the MCI/MCDI frame in ID3v2.
     *
     * @param data the serialized TOC data
     * @return TRUE if the TOC was successfully deserialized
     */
    bool deserialize(const std::string& data) {
	return deserialize(reinterpret_cast<const uint8_t *>(data.data()),
			   data.length());
    };  /* TOC::deserialize(const std::string&) */

    /**
     * Serialize a TOC instance for later deserialization with
     * TOC::deserialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0000b, and is compatible with the (poorly defined) definition
     *       of the MCI/MCDI frame in ID3v2.
     *
     * @return the serialized TOC if successfully serialized; otherwise the
     *         empty string will be returned, and errorCode() will be set
     *         appropriately
     */
    std::string serialize() { CUEIFY_SERIALIZER(cueify_toc_serialize, _t); };

    /**
     * Get the number of the first track.
     *
     * @return the number of the first track
     */
    uint8_t firstTrack() const { return cueify_toc_get_first_track(_t); };

    /**
     * Get the number of the last track.
     *
     * @return the number of the last track
     */
    uint8_t lastTrack() const { return cueify_toc_get_last_track(_t); };

    /** A track in the table of contents (TOC) of a CD. */
    class Track {
    protected:
	const TOC* _t;
	uint8_t _track;
    public:
	Track() : _t(NULL), _track(0) {}

	/**
	 * Create a new track in a TOC.
	 *
	 * @param t the TOC in which to create the track
	 * @param track the track number to create the track for
	 */
	Track(const TOC*  t, uint8_t track) : _t(t), _track(track) { }

	/**
	 * Get the track control flags.
	 *
	 * @return the track control flags
	 */
	uint8_t controlFlags() const {
	    return cueify_toc_get_track_control_flags(_t->_t, _track);
	};  /* TOC::Track::controlFlags */

	/**
	 * Get the format of the content of the sub-Q-channel in the block in
	 * which a track was found.
	 *
	 * @return the format of the contents of the sub-Q-channel
	 */
	uint8_t subQChannelFormat() const {
	    return cueify_toc_get_track_sub_q_channel_format(_t->_t, _track);
	};  /* TOC::Track::subQChannelFormat */

	/**
	 * Get the absolute CD-frame address (LBA) of the start of a track.
	 *
	 * @return the address of the start of the track
	 */
	uint32_t address() const {
	    return cueify_toc_get_track_address(_t->_t, _track);
	};  /* TOC::Track::address */

	/**
	 * Get the total number of CD-frames in the track.
	 *
	 * @return the total number of CD-frames in the track
	 */
	uint32_t length() const {
	    return cueify_toc_get_track_length(_t->_t, _track);
	};  /* TOC::Track::length */
    };  /* TOC::Track */

    /**
     * Get the track data for (non-leadout) tracks in the TOC.
     *
     * @return a std::vector of track data for each track in the TOC
     */
    const std::vector<Track> tracks() const {
	int i = 0;
	std::vector<Track> theTracks;

	if (firstTrack() != 0 || lastTrack() != 0) {
	    theTracks.reserve(lastTrack() - firstTrack() + 1);
	    for (i = firstTrack(); i <= lastTrack(); i++) {
		theTracks.push_back(Track(this, i));
	    }
	}

	return theTracks;
    }  /* TOC::tracks */

    /**
     * Get the track data for the leadout track in the TOC.
     *
     * @return the track data for the leadout track
     */
    const Track leadoutTrack() const {
	return Track(this, CUEIFY_LEAD_OUT_TRACK);
    };  /* TOC::leadoutTrack */

    /**
     * Get the total number of CD-frames.
     *
     * @return the total number of CD-frames
     */
    uint32_t discLength() const { return cueify_toc_get_disc_length(_t); }
};  /* TOC */


/** The multi-session data of an audio CD. */
class Sessions {
protected:
    cueify_sessions *_s;
    int _errorCode;
public:
    friend class Device;

    /**
     * Create a new multisession instance. The instance is created
     * with no data, and should be populated using
     * Sessions::deserialize().
     */
    Sessions() : _s(cueify_sessions_new()), _errorCode(CUEIFY_OK) { }
    ~Sessions() { cueify_sessions_free(_s); }

    /**
     * Get the most recent error code from a call to this multisession
     * instance.
     *
     * @return the most recent error code
     */
    int errorCode() const { return _errorCode; };

    /**
     * Deserialize a multisession instance previously serialized with
     * Sessions::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0001b.
     *
     * @pre { buffer != NULL }
     * @param buffer a pointer to the serialized multisession data
     * @param size the size of the buffer
     * @return TRUE if the multisession instance was successfully deserialized
     */
    bool deserialize(const uint8_t * const buffer, size_t size) {
	CUEIFY_DESERIALIZER(cueify_sessions_deserialize, _s);
    };  /* Sessions::deserialize(const uint8_t * const, size_t) */

    /**
     * Deserialize a multisession instance previously serialized with
     * Sessions::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0001b.
     *
     * @param data the serialized multisession data
     * @return TRUE if the multisession instance was successfully deserialized
     */
    bool deserialize(const std::string& data) {
	return deserialize(reinterpret_cast<const uint8_t *>(data.data()),
			   data.length());
    };  /* Sessions::deserialize(const std::string&) */

    /**
     * Serialize a multisession instance for later deserialization with
     * Sessions::deserialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0001b.
     *
     * @return the serialized multisession instance if successfully
     *         serialized; otherwise the empty string will be returned and
     *         errorCode() will be set appropriately
     */
    std::string serialize() {
	CUEIFY_SERIALIZER(cueify_sessions_serialize, _s);
    };  /* Sessions::serialize */

    /**
     * Get the number of the first session.
     *
     * @return the number of the first session
     */
    uint8_t firstSession() const {
	return cueify_sessions_get_first_session(_s);
    };  /* Sessions::firstSession */

    /**
     * Get the number of the last session.
     *
     * @return the number of the last session
     */
    uint8_t lastSession() const {
	return cueify_sessions_get_last_session(_s);
    };  /* Sessions::lastSession */

    /**
     * Get the track control flags for the first track in the last session.
     *
     * @return the control flags for the first track in the last session
     */
    uint8_t lastSessionControlFlags() const {
	return cueify_sessions_get_last_session_control_flags(_s);
    };  /* Sessions::lastSessionControlFlags */

    /**
     * Get the format of the content of the sub-Q-channel for the
     * first track in the last complete session.
     *
     * @return the format of the contents of the sub-Q-channel for the
     *         first track in the last complete session
     */
    uint8_t lastSessionSubQChannelFormat() const {
	return cueify_sessions_get_last_session_sub_q_channel_format(_s);
    };  /* Sessions::lastSessionSubQChannelFormat */

    /**
     * Get the track number of the first track in the last complete
     * session.
     *
     * @return the track number of the first track in the last complete
     *         session
     */
    uint8_t lastSessionTrackNumber() const {
	return cueify_sessions_get_last_session_track_number(_s);
    };  /* Sessions::lastSessionTrackNumber */


    /**
     * Get the absolute CD-frame address (LBA) of the start of the
     * first track in the last complete session.
     *
     * @return the address of the start of the first track in the last
     *         complete session
     */
    uint32_t lastSessionAddress() const {
	return cueify_sessions_get_last_session_address(_s);
    };  /* Sessions::lastSessionAddress */
};  /* Sessions */


/** The full table of contents (TOC) of an audio CD. */
class FullTOC {
protected:
    cueify_full_toc *_t;
    int _errorCode;
public:
    friend class Device;

    /**
     * Create a new full TOC instance. The instance is created with no tracks,
     * and should be populated using FullTOC::deserialize().
     */
    FullTOC() : _t(cueify_full_toc_new()), _errorCode(CUEIFY_OK) { }
    ~FullTOC() { cueify_full_toc_free(_t); }

    /**
     * Get the most recent error code from a call to this full TOC
     * instance.
     *
     * @return the most recent error code
     */
    int errorCode() const { return _errorCode; };

    /**
     * Deserialize a full TOC instance previously serialized with
     * FullTOC::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0010b, excluding any track descriptors with ADR=5.
     *
     * @pre { buffer != NULL }
     * @param buffer a pointer to the serialized full TOC data
     * @param size the size of the buffer
     * @return TRUE if the full TOC was successfully deserialized
     */
    bool deserialize(const uint8_t * const buffer, size_t size) {
	CUEIFY_DESERIALIZER(cueify_full_toc_deserialize, _t);
    };  /* FullTOC::deserialize(const uint8_t * const, size_t) */

    /**
     * Deserialize a full TOC instance previously serialized with
     * FullTOC::serialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0010b, excluding any track descriptors with ADR=5.
     *
     * @param data the serialized full TOC data
     * @return TRUE if the full TOC was successfully deserialized
     */
    bool deserialize(const std::string& data) {
	return deserialize(reinterpret_cast<const uint8_t *>(data.data()),
			   data.length());
    };  /* FullTOC::deserialize(const std::string&) */

    /**
     * Serialize a full TOC instance for later deserialization with
     * FullTOC::deserialize().
     *
     * @note This serialization is, in principle, the same as that
     *       returned by the MMC READ TOC/PMA/ATIP command with format
     *       0010b, excluding any track descriptors with ADR=5.
     *
     * @return the serialized full TOC if successfully serialized;
     *         otherwise the empty string will be returned and errorCode()
     *         will be set appropriately
     */
    std::string serialize() {
	CUEIFY_SERIALIZER(cueify_full_toc_serialize, _t);
    };  /* FullTOC::serialize */

    /**
     * Get the number of the first session.
     *
     * @return the number of the first session
     */
    uint8_t firstSession() const {
	return cueify_full_toc_get_first_session(_t);
    };  /* FullTOC::firstSession */

    /**
     * Get the number of the last session.
     *
     * @return the number of the last session
     */
    uint8_t lastSession() const {
	return cueify_full_toc_get_last_session(_t);
    };  /* FullTOC::lastSession */


    /** A track in the full table of contents (TOC) of a CD. */
    class Track {
    protected:
	const FullTOC* _t;
	uint8_t _track;
    public:
	Track() : _t(NULL), _track(0) { }

	/**
	 * Create a new track in a full TOC.
	 *
	 * @param t the full TOC in which to create the track
	 * @param track the track number to create the track for
	 */
	Track(const FullTOC*  t, uint8_t track) : _t(t), _track(track) { }

	/**
	 * Get the session number.
	 *
	 * @return the session number
	 */
	uint8_t session() const {
	    return cueify_full_toc_get_track_session(_t->_t, _track);
	};  /* FullTOC::Track::session */

	/**
	 * Get the track control flags.
	 *
	 * @return the track control flags
	 */
	uint8_t controlFlags() const {
	    return cueify_full_toc_get_track_control_flags(_t->_t, _track);
	};  /* FullTOC::Track::controlFlags */

	/**
	 * Get the format of the content of the sub-Q-channel in the block in
	 * which a track was found.
	 *
	 * @return the format of the contents of the sub-Q-channel
	 */
	uint8_t subQChannelFormat() const {
	    return cueify_full_toc_get_track_sub_q_channel_format(_t->_t,
								  _track);
	};  /* FullTOC::Track::subQChannelFormat */

	/**
	 * Get the absolute time of track data in the lead-in.
	 *
	 * @return the absolute time of the track data
	 */
	MSFAddress pointAddress() const {
	    return MSFAddress(cueify_full_toc_get_point_address(
		_t->_t,
		cueify_full_toc_get_track_session(_t->_t, _track),
		_track));
	};  /* FullTOC::Track::pointAddress */

	/**
	 * Get the absolute CD-frame address (LBA) of the start of a track.
	 *
	 * @return the address of the start of the track
	 */
	MSFAddress address() const {
	    return MSFAddress(cueify_full_toc_get_track_address(_t->_t,
								_track));
	};  /* FullTOC::Track::address */

	/**
	 * Get the total length of a track.
	 *
	 * @return the length of the track
	 */
	MSFAddress length() const {
	    return MSFAddress(cueify_full_toc_get_track_length(_t->_t,
							       _track));
	};  /* FullTOC::Track::length */
    };  /* FullTOC::Track */

    /**
     * Get the track data for (non-leadout) tracks in the full TOC.
     *
     * @return a std::vector of track data for each track in the full TOC
     */
    const std::vector<Track> tracks() const {
	int i = 0;
	std::vector<Track> theTracks;

	if (firstTrack() != 0 || lastTrack() != 0) {
	    theTracks.reserve(lastTrack() - firstTrack() + 1);
	    for (i = firstTrack(); i <= lastTrack(); i++) {
		theTracks.push_back(Track(this, i));
	    }
	}

	return theTracks;
    }  /* FullTOC::tracks */

    /** A session in the full table of contents (TOC) of a CD. */
    class Session {
    protected:
	const FullTOC* _t;
	uint8_t _session;
    public:
	Session() : _t(NULL), _session(0) {}

	/**
	 * Create a new session in a full TOC.
	 *
	 * @param t the full TOC in which to create the session
	 * @param session the session number to create the session for
	 */
	Session(const FullTOC* t, uint8_t session) :
	    _t(t), _session(session) { }

	class Pseudotrack {
	protected:
	    const Session* _s;
	    uint8_t _point;
	public:
	    Pseudotrack() : _s(NULL), _point(0) { }
	    
	    /**
	     * Create a new pseudotrack in a session in a full TOC.
	     *
	     * @param s the session in which to create the pseudotrack
	     * @param point the point number to create the pseudotrack for
	     */
	    Pseudotrack(const Session* s, uint8_t point) :
		_s(s), _point(point) { }

	    /**
	     * Get the track control flags for the pseudotrack.
	     *
	     * @return the control flags for the pseudotrack
	     */
	    uint8_t controlFlags() const {
		return cueify_full_toc_get_session_control_flags(_s->_t->_t,
								 _s->_session,
								 _point);
	    };  /* FullTOC::Session::Pseudotrack::controlFlags */

	    /**
	     * Get the format of the sub-Q-channel of the block in
	     * which the pseudotrack was found.
	     *
	     * @return the format of the contents of the sub-Q-channel
	     *         for the pseudotrack
	     */
	    uint8_t subQChannelFormat() const {
		return cueify_full_toc_get_session_sub_q_channel_format(
		    _s->_t->_t, _s->_session, _point);
	    };  /* FullTOC::Session::Pseudotrack::subQChannelFormat */

	    /**
	     * Get the absolute time of a pseoudotrack in the lead-in.
	     *
	     * @return the absolute time of point
	     */
	    MSFAddress pointAddress() const {
		return MSFAddress(cueify_full_toc_get_point_address(
				      _s->_t->_t, _s->_session, _point));
	    };  /* FullTOC::Session::Pseudotrack::pointAddress */
	};  /* FullTOC::Session::Pseudotrack */

	/**
	 * Get the track data for pseudotracks in a session in the full TOC.
	 *
	 * @return a std::vector of track data for each pseudotrack in
	 *         the session in the order
	 *         CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK,
	 *         CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK,
	 *         CUEIFY_LEAD_OUT_TRACK.
	 */
	const std::vector<Pseudotrack> pseudotracks() const {
	    std::vector<Pseudotrack> theTracks;

	    theTracks.reserve(3);
	    theTracks.push_back(
		Pseudotrack(this, CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK));
	    theTracks.push_back(
		Pseudotrack(this, CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK));
	    theTracks.push_back(Pseudotrack(this, CUEIFY_LEAD_OUT_TRACK));

	    return theTracks;
	}  /* FullTOC::Session::pseudotracks */

	/**
	 * Get the number of the first track of a session.
	 *
	 * @return the number of the first track in the session
	 */
	uint8_t firstTrack() const {
	    return cueify_full_toc_get_session_first_track(_t->_t, _session);
	};  /* FullTOC::Session::firstTrack */

	/**
	 * Get the number of the last track of a session.
	 *
	 * @return the number of the last track in the session
	 */
	uint8_t lastTrack() const {
	    return cueify_full_toc_get_session_last_track(_t->_t, _session);
	};  /* FullTOC::Session::lastTrack */

	/**
	 * Get the session type.
	 *
	 * @return the session type of the session
	 */
	uint8_t type() const {
	    return cueify_full_toc_get_session_type(_t->_t, _session);
	};  /* FullTOC::Session::type */

	/**
	 * Get the time of the lead-out of a session.
	 *
	 * @return the time of the lead-out address of the session
	 */
	MSFAddress leadoutAddress() const {
	    return MSFAddress(cueify_full_toc_get_session_leadout_address(
				  _t->_t, _session));
	};  /* FullTOC::Session::leadoutAddress */

	/**
	 * Get the total length of a session.
	 *
	 * @return the length of the session
	 */
	MSFAddress length() const {
	    return MSFAddress(cueify_full_toc_get_session_length(_t->_t,
								 _session));
	};  /* FullTOC::Session::length */
    };  /* FullTOC::Session */

    /**
     * Get the session data for sessions in the full TOC.
     *
     * @return a std::vector of session data for each session in the full TOC
     */
    const std::vector<Session> sessions() const {
	int i = 0;
	std::vector<Session> theSessions;

	if (firstSession() != 0 || lastSession() != 0) {
	    theSessions.reserve(lastSession() - firstSession() + 1);
	    for (i = firstSession(); i <= lastSession(); i++) {
		theSessions.push_back(Session(this, i));
	    }
	}

	return theSessions;
    }  /* FullTOC::sessions */

    /**
     * Get the number of the first track.
     *
     * @return the number of the first track
     */
    uint8_t firstTrack() const {
	return cueify_full_toc_get_first_track(_t);
    };  /* FullTOC::firstTrack */

    /**
     * Get the number of the last track.
     *
     * @return the number of the last track
     */
    uint8_t lastTrack() const {
	return cueify_full_toc_get_last_track(_t);
    };  /* FullTOC::lastTrack */

    /**
     * Get the total length of a CD.
     *
     * @return the total length of a CD
     */
    MSFAddress discLength() const {
	return MSFAddress(cueify_full_toc_get_disc_length(_t));
    };  /* FullTOC::discLength */
};  /* FullTOC */


/** The CD-Text of an audio CD. */
class CDText {
protected:
    cueify_cdtext *_t;
    int _errorCode;
public:
    friend class Device;

    /**
     * Create a new CD-Text instance. The instance is created with no tracks,
     * and should be populated using CDText::deserialize().
     */
    CDText() : _t(cueify_cdtext_new()), _errorCode(CUEIFY_OK) { }
    ~CDText() { cueify_cdtext_free(_t); }

    /**
     * Get the most recent error code from a call to this CD-Text
     * instance.
     *
     * @return the most recent error code
     */
    int errorCode() const { return _errorCode; };

    /**
     * Deserialize a CD-Text instance previously serialized with
     * CDText::serialize().
     *
     * @note This serialization is, in principle, the same as that on the
     *       disc itself.
     *
     * @pre { buffer != NULL }
     * @param buffer a pointer to the serialized CD-Text data
     * @param size the size of the buffer
     * @return TRUE if the CD-Text was successfully deserialized
     */
    bool deserialize(const uint8_t * const buffer, size_t size) {
	CUEIFY_DESERIALIZER(cueify_cdtext_deserialize, _t);
    };  /* CDText::deserialize(const uint8_t * const, size_t) */

    /**
     * Deserialize a CD-Text instance previously serialized with
     * CDText::serialize().
     *
     * @note This serialization is, in principle, the same as that on the
     *       disc itself.
     *
     * @param data the serialized CD-Text data
     * @return TRUE if the CD-Text was successfully deserialized
     */
    bool deserialize(const std::string& data) {
	return deserialize(reinterpret_cast<const uint8_t *>(data.data()),
			   data.length());
    };  /* CDText::deserialize(const std::string&) */

    /**
     * Serialize a CD-Text instance for later deserialization with
     * CDText::deserialize().
     *
     * @note This serialization is, in principle, the same as that on the
     *       disc itself.
     *
     * @return the serialized CD-Text if successfully serialized;
     *         otherwise the empty string will be returned and errorCode()
     *         will be set appropriately
     */
    std::string serialize() {
	CUEIFY_SERIALIZER(cueify_cdtext_serialize, _t);
    };  /* CDText::serialize */

    /** The table of contents (TOC) stored in CD-Text. */
    class TOC {
    protected:
	const CDText *_t;
    public:
	TOC() : _t(NULL) { }

	/**
	 * Create a new TOC in a CD-Text block.
	 *
	 * @param t the CD-Text to which to add the TOC
	 */
	TOC(const CDText* t) : _t(t) { }

        /**
	 * Get the number of the first track in the TOC.
	 *
	 * @return the number of the first track in the TOC
	 */
	uint8_t firstTrack() const {
	    return cueify_cdtext_get_toc_first_track(_t->_t);
	};  /* CDText::TOC::firstTrack */

        /**
	 * Get the number of the last track in the TOC.
	 *
	 * @return the number of the last track in the TOC
	 */
	uint8_t lastTrack() const {
	    return cueify_cdtext_get_toc_last_track(_t->_t);
	};  /* CDText::TOC::lastTrack */

	/** A track in the CD-Text TOC. */
	class Track {
	protected:
	    const TOC* _t;
	    uint8_t _track;
	public:
	    Track() : _t(NULL), _track(0) { }

	    /**
	     * Add a track to a CD-Text TOC.
	     *
	     * @param t the TOC to add a track to
	     * @param track the number of the track to add to the TOC
	     */
	    Track(const TOC* t, uint8_t track) : _t(t), _track(track) { }

	    /** A track interval in a CD-Text TOC. */
	    class Interval {
	    protected:
		const Track* _t;
		uint8_t _interval;
	    public:
		Interval() : _t(NULL), _interval(0) { }

		/**
		 * Add an interval to a CD-Text TOC track.
		 *
		 * @param t the track to add an interval to
		 * @param interval the number of the interval in the track
		 */
		Interval(const Track* t, uint8_t interval) :
		    _t(t), _interval(interval) { }

                /**
		 * Get the start address of a track interval in the
		 * CD-Text TOC.
		 *
		 * @return the start address of the track interval
		 */
		MSFAddress start() const {
		    return MSFAddress(
			cueify_cdtext_get_toc_track_interval_start(
			    _t->_t->_t->_t, _t->_track, _interval));
		};  /* CDText::TOC::Track::Interval::start */

                /**
		 * Get the end address of a track interval in the
		 * CD-Text TOC.
		 *
		 * @return the end address of the track interval
		 */
		MSFAddress end() const {
		    return MSFAddress(
			cueify_cdtext_get_toc_track_interval_end(
			    _t->_t->_t->_t, _t->_track, _interval));
		};  /* CDText::TOC::Track::Interval::end */
	    };  /* CDText::TOC::Track::Interval */

            /**
	     * Get the track intervals for a track in the CD-Text TOC.
	     *
	     * @return an std::vector containing the track intervals
	     *         in the TOC
	     */
	    const std::vector<Interval> intervals() const {
		int i = 0;
		std::vector<Interval> theIntervals;

		theIntervals.reserve(
		    cueify_cdtext_get_toc_num_track_intervals(_t->_t->_t,
							      _track));
		for (i = 1;
		     i <= cueify_cdtext_get_toc_num_track_intervals(_t->_t->_t,
								    _track);
		     i++) {
		    theIntervals.push_back(Interval(this, i));
		}

		return theIntervals;
	    };  /* CDText::TOC::Track::intervals */

            /**
	     * Get the time of the start address of a track in the
	     * CD-Text TOC.
	     *
	     * @return the time of the start address of the track
	     */
	    MSFAddress offset() const {
		return MSFAddress(
		    cueify_cdtext_get_toc_track_offset(_t->_t->_t, _track));
	    };  /* CDText::TOC::Track::offset */
	};  /* CDText::TOC::Track */

        /**
	 * Get the tracks in the CD-Text TOC.
	 *
	 * @return an std::vector containing the tracks in the TOC
	 */
	const std::vector<Track> tracks() const {
	    int i = 0;
	    std::vector<Track> theTracks;

	    if (firstTrack() != 0 || lastTrack() != 0) {
		theTracks.reserve(lastTrack() - firstTrack() + 1);
		for (i = firstTrack(); i <= lastTrack(); i++) {
		    theTracks.push_back(Track(this, i));
		}
	    }

	    return theTracks;
	};  /* CDText::TOC::tracks */
    };  /* CDText::TOC */

    /**
     * Get the TOC in the CD-Text.
     *
     * @return the TOC in the CD-Text
     */
    const TOC toc() const { return TOC(this); }

    /** A block of CD-Text data on an audio CD. */
    class Block {
    protected:
	cueify_cdtext_block *_b;

	/* Workaround for SWIG temporary variable bug. */
	std::string _discid, _genreName;
    public:
	Block() : _b(NULL) { }

	/**
	 * Create a block from a CD-Text instance.
	 *
	 * @param b the cueify_cdtext_block instance to instantiate
	 */
	Block(cueify_cdtext_block *b) : _b(b) { }

        /**
	 * Get the number of the first track.
	 *
	 * @return the number of the first track in the block
	 */
	uint8_t firstTrack() const {
	    return cueify_cdtext_block_get_first_track(_b);
	};  /* CDText::Block::firstTrack */

        /**
	 * Get the number of the last track.
	 *
	 * @return the number of the last track in the block
	 */
	uint8_t lastTrack() const {
	    return cueify_cdtext_block_get_last_track(_b);
	};  /* CDText::Block::lastTrack */

        /**
	 * Get the character set used to encode data in a CD-Text block.
	 *
	 * @return the character set used to encode the metadata
	 */
	uint8_t charset() const {
	    return cueify_cdtext_block_get_charset(_b);
	};  /* CDText::Block::charset */

        /**
	 * Get the language used to encode data in a CD-Text block.
	 *
	 * @return the language used to encode the metadata
	 */
	uint8_t language() const {
	    return cueify_cdtext_block_get_language(_b);
	};  /* CDText::Block::language */

        /**
	 * Get whether or not CD-Text data is available in program
	 * data on this disc.
	 *
	 * @return TRUE if the data is available in Mode 2 packets in
	 *         program data on this disc, otherwise FALSE
	 */
	bool hasProgramData() const {
	    return cueify_cdtext_block_has_program_data(_b);
	};  /* CDText::Block::hasProgramData */

        /**
	 * Get whether or not CD-Text data in the program track is copyrighted.
	 *
	 * @return TRUE if the data is copyrighted in the program
	 *         data, otherwise FALSE
	 */
	bool hasProgramCopyright() const {
	    return cueify_cdtext_block_has_program_copyright(_b);
	};  /* CDText::Block::hasProgramCopyright */

        /**
	 * Get whether or not the message in a CD-Text block is copyrighted.
	 *
	 * @return TRUE if the message data is copyrighted, otherwise FALSE
	 */
	bool hasMessageCopyright() const {
	    return cueify_cdtext_block_has_message_copyright(_b);
	};  /* CDText::Block::hasMessageCopyright */

        /**
	 * Get whether or not the artist names in a CD-Text block are
	 * copyrighted.
	 *
	 * @return TRUE if the artist names are copyrighted, otherwise FALSE
	 */
	bool hasNameCopyright() const {
	    return cueify_cdtext_block_has_name_copyright(_b);
	};  /* CDText::Block::hasNameCopyright */

        /**
	 * Get whether or not the titles in a CD-Text block are copyrighted.
	 *
	 * @return TRUE if the titles are copyrighted, otherwise FALSE
	 */
	bool hasTitleCopyright() const {
	    return cueify_cdtext_block_has_title_copyright(_b);
	};  /* CDText::Block::hasTitleCopyright */

	/** Track data in a CD-Text block. */
	class Track {
	protected:
	    const Block *_b;
	    uint8_t _track;

	    /* Workaround for SWIG temporary variable bug. */
	    std::string _title, _performer, _songwriter, _composer, _arranger,
		_message, _privateData, _upcISRC;
	public:
	    Track() : _b(NULL), _track(0) { }

	    /**
	     * Add a track to a CD-Text block.
	     *
	     * @param b the block to which to add the track
	     * @param track the number of the track to add (or
	     *        CUEIFY_CDTEXT_ALBUM for the album)
	     */
	    Track(const Block *b, uint8_t track) : _b(b), _track(track) { }

#define CUEIFY_CDTEXT_TESTER(getter)		\
	    return getter(_b->_b, _track) != NULL
#define CUEIFY_CDTEXT_GETTER(getter, variable)				\
	    const char *datum = getter(_b->_b, _track);			\
	    if (datum == NULL) {					\
	        variable = std::string();				\
	    } else {							\
		variable = std::string(datum);				\
	    }								\
	    return variable;

	    /**
	     * Get whether or not the title of the track is set.
	     *
	     * @return TRUE if the title is set
	     */
	    bool hasTitle() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_title);
	    };  /* CDText::Block::Track::hasTitle */

	    /**
	     * Get the title of the track.
	     *
	     * @return the title of the track, or the empty string if it is
	     *         not set
	     */
	    const std::string& title() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_title, _title);
	    };  /* CDText::Block::Track::title */

	    /**
	     * Get whether or not the performer of the track is set.
	     *
	     * @return TRUE if the performer is set
	     */
	    bool hasPerformer() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_performer);
	    };  /* CDText::Block::Track::hasPerformer */

	    /**
	     * Get the performer of the track.
	     *
	     * @return the performer of the track, or the empty string if it is
	     *         not set
	     */
	    const std::string& performer() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_performer,
				     _performer);
	    };  /* CDText::Block::Track::performer */

	    /**
	     * Get whether or not the songwriter of the track is set.
	     *
	     * @return TRUE if the songwriter is set
	     */
	    bool hasSongwriter() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_songwriter);
	    };  /* CDText::Block::Track::hasSongwriter */

	    /**
	     * Get the songwriter of the track.
	     *
	     * @return the songwriter of the track, or the empty
	     *         string if it is not set
	     */
	    const std::string& songwriter() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_songwriter,
				     _songwriter);
	    };  /* CDText::Block::Track::songwriter */

	    /**
	     * Get whether or not the composer of the track is set.
	     *
	     * @return TRUE if the composer is set
	     */
	    bool hasComposer() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_composer);
	    };  /* CDText::Block::Track::hasComposer */

	    /**
	     * Get the composer of the track.
	     *
	     * @return the composer of the track, or the empty string if it is
	     *         not set
	     */
	    const std::string& composer() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_composer,
				     _composer);
	    };  /* CDText::Block::Track::composer */

	    /**
	     * Get whether or not the arranger of the track is set.
	     *
	     * @return TRUE if the arranger is set
	     */
	    bool hasArranger() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_arranger);
	    };  /* CDText::Block::Track::hasArranger */

	    /**
	     * Get the arranger of the track.
	     *
	     * @return the arranger of the track, or the empty string if it is
	     *         not set
	     */
	    const std::string& arranger() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_arranger,
				     _arranger);
	    };  /* CDText::Block::Track::arranger */

	    /**
	     * Get whether or not the message of the track is set.
	     *
	     * @return TRUE if the message is set
	     */
	    bool hasMessage() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_message);
	    };  /* CDText::Block::Track::hasMessage */

	    /**
	     * Get the message of the track.
	     *
	     * @return the message of the track, or the empty string if it is
	     *         not set
	     */
	    const std::string& message() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_message, _message);
	    };  /* CDText::Block::Track::message */

	    /**
	     * Get whether or not the private data of the track is set.
	     *
	     * @return TRUE if the private data is set
	     */
	    bool hasPrivateData() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_private);
	    };  /* CDText::Block::Track::hasPrivateData */

	    /**
	     * Get the private data of the track.
	     *
	     * @return the private data of the track, or the empty
	     *         string if it is not set
	     */
	    const std::string& privateData() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_private,
				     _privateData);
	    };  /* CDText::Block::Track::privateData */

	    /**
	     * Get whether or not the UPC/ISRC of the track is set.
	     *
	     * @return TRUE if the UPC/ISRC is set
	     */
	    bool hasUPCISRC() const {
		CUEIFY_CDTEXT_TESTER(cueify_cdtext_block_get_upc_isrc);
	    };  /* CDText::Block::Track::hasUPCISRC */

	    /**
	     * Get the UPC/ISRC of the track.
	     *
	     * @return the UPC/ISRC of the track, or the empty
	     *         string if it is not set
	     */
	    const std::string& upcISRC() {
		CUEIFY_CDTEXT_GETTER(cueify_cdtext_block_get_upc_isrc,
				     _upcISRC);
	    };  /* CDText::Block::Track::upcISRC */
	};  /* CDText::Block::Track */

        /**
	 * Get the tracks in a CD-Text block.
	 *
	 * @return an std::vector containing the tracks in the block
	 *         starting at index 1 (index 0 (=CUEIFY_CDTEXT_ALBUM)
	 *         contains the album data).
	 */
	const std::vector<Track> tracks() const {
	    int i = 0;
	    std::vector<Track> theTracks;

	    if (firstTrack() != 0 || lastTrack() != 0) {
		theTracks.reserve(lastTrack() - firstTrack() + 2);
		theTracks.push_back(Track(this, CUEIFY_CDTEXT_ALBUM));
		for (i = firstTrack(); i <= lastTrack(); i++) {
		    theTracks.push_back(Track(this, i));
		}
	    }

	    return theTracks;
	};  /* CDText::Block::tracks */

	/**
	 * Get whether or not the discid is set.
	 *
	 * @return TRUE if the discid is set
	 */
	bool hasDiscid() const {
	    return cueify_cdtext_block_get_discid(_b) != NULL;
	};  /* CDText::Block::hasDiscid */

	/**
	 * Get the discid.
	 *
	 * @return the discid, or the empty string if it is not set
	 */
	const std::string& discid() {
	    const char *datum = cueify_cdtext_block_get_discid(_b);
	    if (datum == NULL) {
		_discid = std::string();
	    } else {
		_discid = std::string(datum);
	    }
	    return _discid;
	};  /* CDText::Block::discid */

	/**
	 * Get the genre code.
	 *
	 * @return the genre code, or 0 if it is not set
	 */
	uint16_t genreCode() const {
	    return cueify_cdtext_block_get_genre_code(_b);
	};  /* CDText::Block::genreCode */

	/**
	 * Get the genre name.
	 *
	 * @return the genre name, or the empty string if it is not
	 *         set.  A genre code of 0 implies that the string is
	 *         not merely empty, but is truly unset.
	 */
	const std::string& genreName() {
	    const char *datum = cueify_cdtext_block_get_genre_name(_b);
	    if (datum == NULL) {
		_genreName = std::string();
	    } else {
		_genreName = std::string(datum);
	    }
	    return _genreName;
	};  /* CDText::Block::genreName */
    };  /* CDText::Block */

    /**
     * Get the blocks in a CD-Text instance.
     *
     * @return an std::vector containing the blocks in the CD-Text instance.
     *         It may be assumed that the blocks are compacted in strictly
     *         increasing order from index 0 to 7.
     */
    const std::vector<Block> blocks() const {
	int i = 0;
	std::vector<Block> theBlocks;

	theBlocks.reserve(cueify_cdtext_get_num_blocks(_t));
	for (i = 0; i < cueify_cdtext_get_num_blocks(_t); i++) {
	    theBlocks.push_back(Block(cueify_cdtext_get_block(_t, i)));
	}

	return theBlocks;
    };  /* CDText::blocks */
};  /* CDText */


/** An index of a track on an audio CD. */
class TrackIndex {
public:
    /** The number of the index. */
    uint8_t number;
    /** The address of the index. */
    MSFAddress offset;

    TrackIndex() : number(0), offset(MSFAddress()) { }

    /**
     * Create a new instance of a track index.
     *
     * @param indexNumber the number of the index
     * @param indexOffset the address of the index
     */
    TrackIndex(uint8_t indexNumber, MSFAddress indexOffset) :
	number(indexNumber), offset(indexOffset) { }
};  /* TrackIndices */


/** An optical disc (CD-ROM) device. */
class Device {
protected:
    cueify_device *_d;
    int _errorCode;
public:
    /**
     * Create a new handle for the optical disc (CD-ROM) device
     * returned by Device::defaultDevice() and open it.
     */
    Device() {
	_d = cueify_device_new();
	if (_d != NULL && cueify_device_open(_d, NULL) != CUEIFY_OK) {
	    cueify_device_free(_d);
	    _d = NULL;
	}
    };  /* Device::Device() */

    /**
     * Create a new handle for an optical disc (CD-ROM) device and
     * open it.
     *
     * @param device an operating-system-specific device identifier of
     *               the device to open.
     */
    Device(const std::string& device) {
	_d = cueify_device_new();
	if (_d != NULL &&
	    (_errorCode == cueify_device_open(_d,
					      device.c_str())) != CUEIFY_OK) {
	    cueify_device_free(_d);
	    _d = NULL;
	}
    };  /* Device::Device(const std::string&) */

    ~Device() {
	if (_d != NULL) {
	    cueify_device_close(_d);
	    cueify_device_free(_d);
	}
    };  /* Device::~Device */

    /**
     * Return a bitmask of libcueify APIs this device supports on this
     * operating system.
     *
     * @note The returned bitmask can only guarantee that unsupported APIs
     * do not work.  Certain APIs cannot be determined to be supported
     * without testing against a disk known to return data for them.
     *
     * @return a bitmask of supported APIs
     */
    int supportedAPIs() const { return cueify_device_get_supported_apis(_d); };

    /**
     * Get an operating-system-specific device identifier for the default
     * optical disc (CD-ROM) device in this system.
     *
     * @note This function is not guaranteed to be thread-safe (e.g. on Darwin)
     *
     * @return the empty string if there is no default optical disc
     *         (CD-ROM) device in this system, else the identifier of
     *         the default device
     */
    static const std::string defaultDevice() {
	if (cueify_device_get_default_device() == NULL) {
	    return std::string();
	}  else {
	    return std::string(cueify_device_get_default_device());
	}
    };  /* Device::defaultDevice */

    /**
     * Get the most recent error code from a call to this Device.
     *
     * @return the most recent error code
     */
    int errorCode() { return _errorCode; };

    /**
     * Read the TOC of the disc in the optical disc device.  The
     * returned value must be freed.
     *
     * @return a pointer to the TOC, if successfully read; otherwise NULL
     */
    TOC* readTOC() {
	TOC *t = new TOC();
	_errorCode = CUEIFY_ERR_NOMEM;
	if (t == NULL ||
	    (_errorCode = cueify_device_read_toc(_d, t->_t)) == CUEIFY_OK) {
	    return t;
	} else {
	    delete t;
	    return NULL;
	}
    };  /* Device::readTOC */

    /**
     * Read the multisession data of the disc in the optical disc
     * device.  The returned value must be freed.
     *
     * @return a pointer to the multisession data, if successfully
     *         read; otherwise NULL
     */
    Sessions* readSessions() {
	Sessions *s = new Sessions();
	_errorCode = CUEIFY_ERR_NOMEM;
	if (s == NULL ||
	    (_errorCode = cueify_device_read_sessions(_d,
						      s->_s)) == CUEIFY_OK) {
	    return s;
	} else {
	    delete s;
	    return NULL;
	}
    };  /* Device::readSessions */

    /**
     * Read the full TOC of the disc in the optical disc device.  The
     * returned value must be freed.
     *
     * @return a pointer to the full TOC, if successfully read; otherwise NULL
     */
    FullTOC* readFullTOC() {
	FullTOC *t = new FullTOC();
	_errorCode = CUEIFY_ERR_NOMEM;
	if (t == NULL ||
	    (_errorCode = cueify_device_read_full_toc(_d,
						      t->_t)) == CUEIFY_OK) {
	    return t;
	} else {
	    delete t;
	    return NULL;
	}
    };  /* Device::readFullTOC */

    /**
     * Read the CD-Text data of the disc in the optical disc device.  The
     * returned value must be freed.
     *
     * @return a pointer to the CD-Text data, if successfully read;
     *         otherwise NULL
     */
    CDText* readCDText() {
	CDText *t = new CDText();
	_errorCode = CUEIFY_ERR_NOMEM;
	if (t == NULL ||
	    (_errorCode = cueify_device_read_cdtext(_d,
						    t->_t)) == CUEIFY_OK) {
	    return t;
	} else {
	    delete t;
	    return NULL;
	}
    };  /* Device::readCDText */

    /**
     * Read the Media Catalog Number of the disc in the optical disc
     * device.
     *
     * @return the media catalog number if successfully read; otherwise the
     *         empty string will be returned, and errorCode() will be set
     *         appropriately
     */
    const std::string readMCN() {
	char buffer[17] = "";
	size_t size = 17;
	std::string mcn;

	if ((_errorCode = cueify_device_read_mcn(_d, buffer, &size))
	    != CUEIFY_OK) {
	    return std::string();
	}
	return std::string(buffer);
    };  /* Device::readMCN */

    /**
     * Read the International Standard Recording Code (ISRC) of a track in
     * the disc in the optical disc device.
     *
     * @pre { track in range of track numbers on disc }
     * @param track the number of the track for which the ISRC should be
     *              retrieved
     * @return the ISRC if successfully read; otherwise the
     *         empty string will be returned, and errorCode() will be set
     *         appropriately
     */
    const std::string readISRC(uint8_t track) {
	char buffer[17] = "";
	size_t size = 17;
	std::string mcn;

	if ((_errorCode = cueify_device_read_isrc(_d, track, buffer, &size))
	    != CUEIFY_OK) {
	    return std::string();
	}
	return std::string(buffer);
    };  /* Device::readISRC */

    /**
     * Read the indices from a track of the disc in the optical disc
     * device.  These indices may include the pregap of the next track
     * (with a final index having number 0).
     *
     * @pre { track in range of track numbers on disc }
     * @param track the number of the track for which the indices should be
     *              retrieved
     * @return an std::vector containing the track indices (or an empty vector
     *         on failure)
     */
    const std::vector<TrackIndex> readTrackIndices(uint8_t track) {
	cueify_indices *i = cueify_indices_new();
	int j;
	std::vector<TrackIndex> indices;
	_errorCode = CUEIFY_ERR_NOMEM;
	if (i != NULL &&
	    (_errorCode = cueify_device_read_track_indices(_d, i, track))
	    == CUEIFY_OK) {
	    for (j = 0; j < cueify_indices_get_num_indices(i); j++) {
		indices.push_back(
		    TrackIndex(
			cueify_indices_get_index_number(i, j),
			MSFAddress(cueify_indices_get_index_offset(i, j))));
	    }
	}
	if (i != NULL) {
	    cueify_indices_free(i);
	}
	return indices;
    };  /* Device::readTrackIndices */

    /**
     * Read the data mode from a track of the disc in the optical disc
     * device.
     *
     * @pre { track in range of track numbers on disc }
     * @param track the number of the track for which the mode should be
     *              retrieved
     * @return the mode of track number track if the mode was successfully
     *         read; otherwise CUEIFY_DATA_MODE_ERROR is returned
     */
    int readDataMode(uint8_t track) {
	return cueify_device_read_data_mode(_d, track);
    };  /* Device::readDataMode */

    /**
     * Read the control flags of a track on a disc in an optical disc
     * (CD-ROM) device.  This function reads the flags directly from
     * the track rather than relying on the TOC.
     *
     * @pre { track in range of track numbers on disc }
     * @param track the number of the track for which control flags should be
     *              retrieved
     * @return the control flags for track number track (or 0xF if
     *         they could not be read).
     */
    uint8_t readTrackControlFlags(uint8_t track) {
	return cueify_device_read_track_control_flags(_d, track);
    };  /* Device::readTrackControlFlags */
};  /* Device */

}  /* namespace cueify */

#endif  /* _CUEIFY_CUEIFY_HPP */

