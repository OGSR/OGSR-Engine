#pragma once

class trivial_encryptor {
  protected:
  typedef u8 type;
  typedef void* pvoid;
  typedef const void* pcvoid;

  static constexpr u32 alphabet_size = u32( 1 << ( 8 * sizeof( type ) ) );

  class random32 {
protected:
    u32 m_seed;

public:
    IC void seed( const u32& seed ) {
      m_seed = seed;
    }

    IC u32 random( const u32& range ) {
      m_seed = 0x08088405 * m_seed + 1;
      return ( u32( u64( m_seed ) * u64( range ) >> 32 ) );
    }
  };

  protected:
  static u32 m_table_iterations, m_table_seed, m_encrypt_seed;
  static type m_alphabet_back[ alphabet_size ];

  static IC void initialize() {
    if ( !strstr( GetCommandLine(), "-steam" ) ) {
      m_table_iterations = 2048;
      m_table_seed       = 20091958;
      m_encrypt_seed     = 20031955;
    } else {
      m_table_iterations = 1024;
      m_table_seed       = 6011979;
      m_encrypt_seed     = 24031979;
    }

    auto m_alphabet = ( type* )_alloca( sizeof( type ) * alphabet_size );

    for ( u32 i = 0; i < alphabet_size; ++i )
      m_alphabet[ i ] = ( type )i;

    random32 temp;
    temp.seed( m_table_seed );
    for ( u32 i = 0; i < m_table_iterations; ++i ) {
      u32 j = temp.random( alphabet_size );
      u32 k = temp.random( alphabet_size );
      while ( j == k )
        k = temp.random( alphabet_size );

      std::swap( m_alphabet[ j ], m_alphabet[ k ] );
    }

    for ( u32 i = 0; i < alphabet_size; ++i )
      m_alphabet_back[ m_alphabet[ i ] ] = ( type )i;
  }

  public:
  static IC void decode( pcvoid source, const u32& source_size, pvoid destination ) {
    static bool m_initialized = false;
    if ( !m_initialized ) {
      initialize();
      m_initialized = true;
    }

    random32 temp;
    temp.seed( m_encrypt_seed );
    const u8* I = ( const u8* )source;
    const u8* E = ( const u8* )source + source_size;
    u8* J       = ( u8* )destination;
    for ( ; I != E; ++I, ++J )
      *J = m_alphabet_back[ ( *I ) ^ type( temp.random( 256 ) & 0xff ) ];
  }
};

u32 trivial_encryptor::m_table_iterations, trivial_encryptor::m_table_seed, trivial_encryptor::m_encrypt_seed;
trivial_encryptor::type trivial_encryptor::m_alphabet_back[ trivial_encryptor::alphabet_size ];
