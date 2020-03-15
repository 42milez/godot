#ifndef NETWORKED_MULTIPLAYER_ENET_H
#define NETWORKED_MULTIPLAYER_ENET_H

#include "core/io/compression.h"
#include "core/io/networked_multiplayer_peer.h"

#include <enet/enet.h>

class NetworkedMultiplayerENet : public NetworkedMultiplayerPeer { GDCLASS(NetworkedMultiplayerENet, NetworkedMultiplayerPeer);
public:
	enum CompressionMode {
		COMPRESS_NONE,
		COMPRESS_RANGE_CODER,
		COMPRESS_FASTLZ,
		COMPRESS_ZLIB,
		COMPRESS_ZSTD
	};

protected:
	static void _bind_methods();

public:
    /* ⭐ */ NetworkedMultiplayerENet();
	/* ⭐ */ ~NetworkedMultiplayerENet();

    /* ⭐ */ Error create_client(const String &p_address, int p_port, int p_in_bandwidth = 0, int p_out_bandwidth = 0, int p_client_port = 0);
	/* ⭐ */ Error create_server(int p_port, int p_max_clients = 32, int p_in_bandwidth = 0, int p_out_bandwidth = 0);

	/* ⭐ */ void close_connection(uint32_t wait_usec = 100);
	/* ⭐ */ void disconnect_peer(int p_peer, bool now = false);
	
	/* ⭐ */ virtual void poll();

	/* ⭐ */ virtual Error get_packet(const uint8_t **r_buffer, int &r_buffer_size); ///< buffer is GONE after next get_packet
	/* ⭐ */ virtual Error put_packet(const uint8_t *p_buffer, int p_buffer_size);

	bool is_always_ordered() const;
	virtual bool is_refusing_new_connections() const;
	virtual bool is_server() const;

public:
	virtual int get_available_packet_count() const;
	int get_channel_count() const;
	CompressionMode get_compression_mode() const;
	virtual ConnectionStatus get_connection_status() const;
	int get_last_packet_channel() const;
	virtual int get_max_packet_size() const;
	int get_packet_channel() const;
	virtual int get_packet_peer() const;
	virtual IP_Address get_peer_address(int p_peer_id) const;
	virtual int get_peer_port(int p_peer_id) const;
	int get_transfer_channel() const;
	virtual TransferMode get_transfer_mode() const;
	virtual int get_unique_id() const;

	void set_always_ordered(bool p_ordered);
	void set_bind_ip(const IP_Address &p_ip);
	void set_channel_count(int p_channel);
	void set_compression_mode(CompressionMode p_mode);
	virtual void set_refuse_new_connections(bool p_enable);
	virtual void set_target_peer(int p_peer);
	void set_transfer_channel(int p_channel);
	virtual void set_transfer_mode(TransferMode p_mode);

private:
	static size_t enet_compress(void *context, const ENetBuffer *inBuffers, size_t inBufferCount, size_t inLimit, enet_uint8 *outData, size_t outLimit);
	static void enet_compressor_destroy(void *context);
	static size_t enet_decompress(void *context, const enet_uint8 *inData, size_t inLimit, enet_uint8 *outData, size_t outLimit);

	uint32_t _gen_unique_id() const;
	void _pop_current_packet();
	void _setup_compressor();

private:
	enum {
		SYSMSG_ADD_PEER,
		SYSMSG_REMOVE_PEER
	};

	enum {
		SYSCH_CONFIG,
		SYSCH_RELIABLE,
		SYSCH_UNRELIABLE,
		SYSCH_MAX
	};

	struct Packet {
		ENetPacket *packet;
		int from;
		int channel;
	};

	Map<int, ENetPeer *> peer_map;

	List<Packet> incoming_packets;

	Vector<uint8_t> src_compressor_mem;
	Vector<uint8_t> dst_compressor_mem;

	IP_Address bind_ip;
	CompressionMode compression_mode;
	ConnectionStatus connection_status;
	Packet current_packet;
	ENetCompressor enet_compressor;
	ENetEvent event;
	ENetHost *host;
	ENetPeer *peer;
	TransferMode transfer_mode;

	uint32_t unique_id;

	int channel_count;
	int target_peer;
	int transfer_channel;

	bool active;
	bool always_ordered;
	bool refuse_connections;
	bool server;
};

VARIANT_ENUM_CAST(NetworkedMultiplayerENet::CompressionMode);

#endif // NETWORKED_MULTIPLAYER_ENET_H
