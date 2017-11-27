//
//  firfilter.c
//  coursework2
//
//  Created by Olly Seber on 18/11/2017.
//

#include "firfilter.h"


/* GLOBALS */

const double g_pi = 3.14159265359;
const double g_tau = 2 * g_pi;


/* TYPE DEFINITIONS */

typedef struct firfilter_struct {
    double *coeffs;
    int numCoeffs;
    double *delayLine;
    int delayLineIndex;
} firFilter;


/* PRIVATE FUNCTION PROTOTYPES */

firErr initDelayLine( firFilter *filter );

void applyBartlettWindow( firFilter *filter );

void applyHanningWindow( firFilter *filter );

void applyHammingWindow( firFilter *filter );

void applyBlackmanWindow( firFilter *filter );

void filterfatalError( firErr code, char *info );


/* FUNCTION DEFINITIONS */

firFilter* createFilter( int order, double *circularBuffer ) {
    firFilter *filter = malloc( sizeof( firFilter ) );
    if ( filter == NULL ) {
        filterfatalError( FILT_MEM_ERR, "Could not allocate filter memory." );
    }
    
    filter->coeffs = calloc ( order + 1, sizeof( double ) );
    if ( filter->coeffs == NULL ) {
        free( filter ); // Tidy up.
        filterfatalError( FILT_MEM_ERR, "Could not allocate coefficient memory." );
    }
    
    filter->numCoeffs = order + 1;
    filter->delayLine = circularBuffer;
    filter->delayLineIndex = 0;
    
    if ( initDelayLine( filter ) != FILT_NO_ERR ) {
        filterfatalError( FILT_MEM_ERR, "Error initialising filter: NULL filter address." );
    }
    
    return filter;
}


firErr destroyFilter( firFilter *filter ) {
    if ( filter == NULL ) {
        return FILT_ARG_NULL;
    }
    if ( filter->coeffs == NULL ) {
        filterfatalError( FILT_MEM_ERR, "Filter coefficients NULL." );
    }
    
    free( filter->coeffs );
    free( filter );
    
    return FILT_NO_ERR;
}


firErr setCoefficients( firFilter *filter, int samplerate, double cutoff, firWindow window ) {
    if ( filter == NULL ) {
        return FILT_ARG_NULL;
    }
    
    double ft = cutoff / samplerate;
    float M = filter->numCoeffs - 1;
    
    for ( int i = 0; i < filter->numCoeffs; ++i ) {
        if ( i == M / 2 ) {
            filter->coeffs[ i ] = 2 * ft;
        }
        else {
            filter->coeffs[ i ] = sin( g_tau * ft * ( i - ( M / 2 ) ) ) / ( g_pi * ( i - ( M / 2 ) ) );
        }
    }
    
    switch ( window ) {
        case WINDOW_RECTANGULAR:
            break;
        case WINDOW_BARTLETT:
            applyBartlettWindow( filter );
            break;
        case WINDOW_HANNING:
            applyHanningWindow( filter );
            break;
        case WINDOW_HAMMING:
            applyHammingWindow( filter );
            break;
        case WINDOW_BLACKMAN:
            applyBlackmanWindow( filter );
            break;
        default:
            break;
    }
    
    return FILT_NO_ERR;
}


firErr processBuffer( firFilter *filter, double *buffer, int numSamples ) {
    if ( filter == NULL || buffer == NULL ) {
        return FILT_ARG_NULL;
    }
    if ( numSamples < 0 ) {
        return FILT_BAD_ARG;
    }
    
    for ( int i; i < numSamples; ++i ) {
        filter->delayLine[ filter->delayLineIndex ] = buffer[ i ];
        for ( int j; j < filter->numCoeffs; ++j ){
            buffer[ i ] = filter->coeffs[ j ] *
                filter->delayLine[ ( filter->delayLineIndex + j ) % filter->numCoeffs ];
            filter->delayLineIndex = ( filter->delayLineIndex + 1 ) % filter->numCoeffs;
        }
    }
    return FILT_NO_ERR;
}


firErr initDelayLine( firFilter *filter ) {
    if ( filter == NULL ) {
        return FILT_ARG_NULL;
    }
    
    for ( int i = 0; i < filter->numCoeffs; ++i ){
        filter->delayLine[ i ] = 0;
    }
    return FILT_NO_ERR;
}


void applyBartlettWindow( firFilter *filter ) {
    float M = filter->numCoeffs - 1;
    for ( int i = 0; i < filter->numCoeffs; ++i ) {
        filter->coeffs[ i ] *= 1 - ( 2 * fabs( i - ( M / 2 ) ) / M );
    }
}


void applyHanningWindow( firFilter *filter ) {
    float M = filter->numCoeffs - 1;
    for ( int i = 0; i < filter->numCoeffs; ++i ) {
        filter->coeffs[ i ] *= 0.5 - ( 0.5 * cos( 2 * g_pi * i / M ) );
    }
}


void applyHammingWindow( firFilter *filter ) {
    float M = filter->numCoeffs - 1;
    for ( int i = 0; i < filter->numCoeffs; ++i ) {
        filter->coeffs[ i ] *= 0.54 - ( 0.46 * cos( 2.0 * g_pi * i / M ) );
    }
}


void applyBlackmanWindow( firFilter *filter ) {
    float M = filter->numCoeffs - 1;
    for ( int i = 0; i < filter->numCoeffs; ++i ) {
        filter->coeffs[ i ] *= 0.42 - ( 0.5 * cos( 2.0 * g_pi * i / M ) ) + ( 0.08 * cos( 4.0 * g_pi * i / M ) );
    }
}


void filterfatalError( firErr code, char *info ) {
    fprintf( stderr, "FIR FILTER ERROR: %s\n", info );
    exit( code );
}


#ifdef FILTER_TESTS

double **getData( firFilter *filter ) {
    return &filter->coeffs;
}

int *getOrder( firFilter *filter ) {
    return &filter->numCoeffs;
}

double *getCoefficients( firFilter *filter ) {
    return filter->coeffs;
}

#endif // FILTER_TESTS
