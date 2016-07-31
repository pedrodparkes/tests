
struct iio_image {
	int dimension;        // 1, 2, 3 or 4
	int sizes[IIO_MAX_DIMENSION];
	int pixel_dimension;
	int type;             // IIO_TYPE_*

	int meta;             // IIO_META_*
	int format;           // IIO_FORMAT_*

	bool contiguous_data;
	bool caca[3];
	void *data;
};

/////////////////////////////

void iio_save_image_float_split(char *filename, float *data,
		int w, int h, int pd)
{
	float *rdata = xmalloc(w*h*pd*sizeof*rdata);
	recover_broken_pixels_float(rdata, data, w*h, pd);
	iio_save_image_float_vec(filename, rdata, w, h, pd);
	xfree(rdata);
}

static void recover_broken_pixels_float(float *clear, float *broken, int n, int pd)
{
	//fprintf(stderr, "unbreaking %d %d-dimensional vectors\n", n, pd);
	FORL(pd) FORI(n)
		clear[pd*i + l] = broken[n*l + i];
}

#define FORL(n) for(int l=0;l<(int)(n);l++)

void iio_save_image_float_vec(char *filename, float *data,
		int w, int h, int pd)
{
	struct iio_image x[1];
	x->dimension = 2;
	x->sizes[0] = w;
	x->sizes[1] = h;
	x->pixel_dimension = pd;
	x->type = IIO_TYPE_FLOAT;
	x->data = data;
	x->contiguous_data = false;
	iio_save_image_default(filename, x);
}

/////////////////////

static void iio_save_image_default(const char *filename, struct iio_image *x)
{
	int typ = normalize_type(x->type);
	if (string_suffix(filename, ".uv") && typ == IIO_TYPE_FLOAT
				&& x->pixel_dimension == 2) {
		iio_save_image_as_juv(filename, x);
		return;
	}
	if (string_suffix(filename, ".flo") && typ == IIO_TYPE_FLOAT
				&& x->pixel_dimension == 2) {
		iio_save_image_as_flo(filename, x);
		return;
	}
	int nsamp = iio_image_number_of_samples(x);
	if (true) {
		char *pngname = strstr(filename, "PNG:");
		if (pngname == filename) {
			if (typ == IIO_TYPE_FLOAT) {
				void *old_data = x->data;
				x->data = xmalloc(nsamp*sizeof(float));
				memcpy(x->data, old_data, nsamp*sizeof(float));
				iio_convert_samples(x, IIO_TYPE_UINT8);
				iio_save_image_default(filename, x);//recursive
				xfree(x->data);
				x->data = old_data;
				return;
			}
			iio_save_image_as_png(filename+4, x);
			return;
		}
	}
	if (true) {
		if (false
				|| string_suffix(filename, ".png")
				|| string_suffix(filename, ".PNG")
				|| (typ==IIO_TYPE_UINT8&&x->pixel_dimension==4)
				|| (typ==IIO_TYPE_UINT8&&x->pixel_dimension==2)
		   )
		{
			if (typ == IIO_TYPE_FLOAT) {
				void *old_data = x->data;
				x->data = xmalloc(nsamp*sizeof(float));
				memcpy(x->data, old_data, nsamp*sizeof(float));
				iio_convert_samples(x, IIO_TYPE_UINT8);
				iio_save_image_default(filename, x);//recursive
				xfree(x->data);
				x->data = old_data;
				return;
			}
			iio_save_image_as_png(filename, x);
			return;
		}
	}
}

/////////////////////



static void iio_convert_samples(struct iio_image *x, int desired_type)
{
	assert(!x->contiguous_data);
	int source_type = normalize_type(x->type);
	if (source_type == desired_type) return;
	IIO_DEBUG("converting from %s to %s\n", iio_strtyp(x->type), iio_strtyp(desired_type));
	int n = iio_image_number_of_samples(x);
	x->data = convert_data(x->data, n, desired_type, source_type);
	x->type = desired_type;
}

static int normalize_type(int type_in)
{
	int type_out;
	switch(type_in) {
	case IIO_TYPE_CHAR: type_out = IIO_TYPE_UINT8; break;
	case IIO_TYPE_SHORT: type_out = IIO_TYPE_INT16; break;
	case IIO_TYPE_INT: type_out = IIO_TYPE_UINT32; break;
	default: type_out = type_in; break;
	}
	if (type_out != type_in) {
		// the following assertion fails on many architectures
		assert(iio_type_size(type_in) == iio_type_size(type_out));
	}
	return type_out;
}


static int iio_image_number_of_samples(struct iio_image *x)
{
	return iio_image_number_of_elements(x) * x->pixel_dimension;
}
static int iio_image_number_of_elements(struct iio_image *x)
{
	iio_image_assert_struct_consistency(x);
	int r = 1;
	FORI(x->dimension) r *= x->sizes[i];
	return r;
}


static void *convert_data(void *src, int n, int dest_fmt, int src_fmt)
{
	if (src_fmt == IIO_TYPE_FLOAT)
		IIO_DEBUG("first float sample = %g\n", *(float*)src);
	size_t src_width = iio_type_size(src_fmt);
	size_t dest_width = iio_type_size(dest_fmt);
	IIO_DEBUG("converting %d samples from %s to %s\n", n, iio_strtyp(src_fmt), iio_strtyp(dest_fmt));
	IIO_DEBUG("src width = %zu\n", src_width);
	IIO_DEBUG("dest width = %zu\n", dest_width);
	char *r = xmalloc(n * dest_width);
	// NOTE: the switch inside "convert_datum" should be optimized
	// outside of this loop
	FORI(n) {
		void *to  = i * dest_width + r;
		void *from = i * src_width + (char *)src;
		convert_datum(to, from, dest_fmt, src_fmt);
	}
	xfree(src);
	if (dest_fmt == IIO_TYPE_INT16)
		IIO_DEBUG("first short sample = %d\n", *(int16_t*)r);
	return r;
}


#define CC(a,b) (77*(a)+(b)) // hash number of a conversion pair
#define I8 IIO_TYPE_INT8
#define U8 IIO_TYPE_UINT8
#define I6 IIO_TYPE_INT16
#define U6 IIO_TYPE_UINT16
#define I2 IIO_TYPE_INT32
#define U2 IIO_TYPE_UINT32
#define I4 IIO_TYPE_INT64
#define U4 IIO_TYPE_UINT64
#define F4 IIO_TYPE_FLOAT
#define F8 IIO_TYPE_DOUBLE
#define F6 IIO_TYPE_LONGDOUBLE
static void convert_datum(void *dest, void *src, int dest_fmt, int src_fmt)
{
	// NOTE: verify that this switch is optimized outside of the loop in
	// which it is contained.  Otherwise, produce some self-modifying code
	// here.
	switch(CC(dest_fmt,src_fmt)) {

	// change of sign (lossless, but changes interpretation)
	case CC(I8,U8): *(  int8_t*)dest = *( uint8_t*)src; break;
	case CC(I6,U6): *( int16_t*)dest = *(uint16_t*)src; break;
	case CC(I2,U2): *( int32_t*)dest = *(uint32_t*)src; break;
	case CC(U8,I8): *( uint8_t*)dest = *(  int8_t*)src; break;
	case CC(U6,I6): *(uint16_t*)dest = *( int16_t*)src; break;
	case CC(U2,I2): *(uint32_t*)dest = *( int32_t*)src; break;

	// different size signed integers (3 lossy, 3 lossless)
	case CC(I8,I6): *(  int8_t*)dest = *( int16_t*)src; break;//iw810
	case CC(I8,I2): *(  int8_t*)dest = *( int32_t*)src; break;//iw810
	case CC(I6,I2): *( int16_t*)dest = *( int32_t*)src; break;//iw810
	case CC(I6,I8): *( int16_t*)dest = *(  int8_t*)src; break;
	case CC(I2,I8): *( int32_t*)dest = *(  int8_t*)src; break;
	case CC(I2,I6): *( int32_t*)dest = *( int16_t*)src; break;

	// different size unsigned integers (3 lossy, 3 lossless)
	case CC(U8,U6): *( uint8_t*)dest = *(uint16_t*)src; break;//iw810
	case CC(U8,U2): *( uint8_t*)dest = *(uint32_t*)src; break;//iw810
	case CC(U6,U2): *(uint16_t*)dest = *(uint32_t*)src; break;//iw810
	case CC(U6,U8): *(uint16_t*)dest = *( uint8_t*)src; break;
	case CC(U2,U8): *(uint32_t*)dest = *( uint8_t*)src; break;
	case CC(U2,U6): *(uint32_t*)dest = *(uint16_t*)src; break;

	// diferent size mixed integers, make signed (3 lossy, 3 lossless)
	case CC(I8,U6): *(  int8_t*)dest = *(uint16_t*)src; break;//iw810
	case CC(I8,U2): *(  int8_t*)dest = *(uint32_t*)src; break;//iw810
	case CC(I6,U2): *( int16_t*)dest = *(uint32_t*)src; break;//iw810
	case CC(I6,U8): *( int16_t*)dest = *( uint8_t*)src; break;
	case CC(I2,U8): *( int32_t*)dest = *( uint8_t*)src; break;
	case CC(I2,U6): *( int32_t*)dest = *(uint16_t*)src; break;

	// diferent size mixed integers, make unsigned (3 lossy, 3 lossless)
	case CC(U8,I6): *( uint8_t*)dest = *( int16_t*)src; break;//iw810
	case CC(U8,I2): *( uint8_t*)dest = *( int32_t*)src; break;//iw810
	case CC(U6,I2): *(uint16_t*)dest = *( int32_t*)src; break;//iw810
	case CC(U6,I8): *(uint16_t*)dest = *(  int8_t*)src; break;
	case CC(U2,I8): *(uint32_t*)dest = *(  int8_t*)src; break;
	case CC(U2,I6): *(uint32_t*)dest = *( int16_t*)src; break;

	// from float (typically lossy, except for small integers)
	case CC(I8,F4): *(  int8_t*)dest = *( float*)src; break;//iw810
	case CC(I6,F4): *( int16_t*)dest = *( float*)src; break;//iw810
	case CC(I2,F4): *( int32_t*)dest = *( float*)src; break;
	case CC(I8,F8): *(  int8_t*)dest = *(double*)src; break;//iw810
	case CC(I6,F8): *( int16_t*)dest = *(double*)src; break;//iw810
	case CC(I2,F8): *( int32_t*)dest = *(double*)src; break;//iw810
	case CC(U8,F4): *( uint8_t*)dest = T8(0.5+*( float*)src); break;//iw810
	case CC(U6,F4): *(uint16_t*)dest = T6(0.5+*( float*)src); break;//iw810
	case CC(U2,F4): *(uint32_t*)dest = *( float*)src; break;
	case CC(U8,F8): *( uint8_t*)dest = T8(0.5+*(double*)src); break;//iw810
	case CC(U6,F8): *(uint16_t*)dest = T6(0.5+*(double*)src); break;//iw810
	case CC(U2,F8): *(uint32_t*)dest = *(double*)src; break;//iw810

	// to float (typically lossless, except for large integers)
	case CC(F4,I8): *( float*)dest = *(  int8_t*)src; break;
	case CC(F4,I6): *( float*)dest = *( int16_t*)src; break;
	case CC(F4,I2): *( float*)dest = *( int32_t*)src; break;//iw810
	case CC(F8,I8): *(double*)dest = *(  int8_t*)src; break;
	case CC(F8,I6): *(double*)dest = *( int16_t*)src; break;
	case CC(F8,I2): *(double*)dest = *( int32_t*)src; break;
	case CC(F4,U8): *( float*)dest = *( uint8_t*)src; break;
	case CC(F4,U6): *( float*)dest = *(uint16_t*)src; break;
	case CC(F4,U2): *( float*)dest = *(uint32_t*)src; break;//iw810
	case CC(F8,U8): *(double*)dest = *( uint8_t*)src; break;
	case CC(F8,U6): *(double*)dest = *(uint16_t*)src; break;
	case CC(F8,U2): *(double*)dest = *(uint32_t*)src; break;

#ifdef I_CAN_HAS_INT64
	// to int64_t and uint64_t
	case CC(I4,I8): *( int64_t*)dest = *(  int8_t*)src; break;
	case CC(I4,I6): *( int64_t*)dest = *( int16_t*)src; break;
	case CC(I4,I2): *( int64_t*)dest = *( int32_t*)src; break;
	case CC(I4,U8): *( int64_t*)dest = *( uint8_t*)src; break;
	case CC(I4,U6): *( int64_t*)dest = *(uint16_t*)src; break;
	case CC(I4,U2): *( int64_t*)dest = *(uint32_t*)src; break;
	case CC(I4,F4): *( int64_t*)dest = *(   float*)src; break;
	case CC(I4,F8): *( int64_t*)dest = *(  double*)src; break;
	case CC(U4,I8): *(uint64_t*)dest = *(  int8_t*)src; break;
	case CC(U4,I6): *(uint64_t*)dest = *( int16_t*)src; break;
	case CC(U4,I2): *(uint64_t*)dest = *( int32_t*)src; break;
	case CC(U4,U8): *(uint64_t*)dest = *( uint8_t*)src; break;
	case CC(U4,U6): *(uint64_t*)dest = *(uint16_t*)src; break;
	case CC(U4,U2): *(uint64_t*)dest = *(uint32_t*)src; break;
	case CC(U4,F4): *(uint64_t*)dest = *(   float*)src; break;
	case CC(U4,F8): *(uint64_t*)dest = *(  double*)src; break;

	// from int64_t and uint64_t
	case CC(I8,I4): *(  int8_t*)dest = *( int64_t*)src; break;
	case CC(I6,I4): *( int16_t*)dest = *( int64_t*)src; break;
	case CC(I2,I4): *( int32_t*)dest = *( int64_t*)src; break;
	case CC(U8,I4): *( uint8_t*)dest = *( int64_t*)src; break;
	case CC(U6,I4): *(uint16_t*)dest = *( int64_t*)src; break;
	case CC(U2,I4): *(uint32_t*)dest = *( int64_t*)src; break;
	case CC(F4,I4): *(   float*)dest = *( int64_t*)src; break;
	case CC(F8,I4): *(  double*)dest = *( int64_t*)src; break;
	case CC(I8,U4): *(  int8_t*)dest = *(uint64_t*)src; break;
	case CC(I6,U4): *( int16_t*)dest = *(uint64_t*)src; break;
	case CC(I2,U4): *( int32_t*)dest = *(uint64_t*)src; break;
	case CC(U8,U4): *( uint8_t*)dest = *(uint64_t*)src; break;
	case CC(U6,U4): *(uint16_t*)dest = *(uint64_t*)src; break;
	case CC(U2,U4): *(uint32_t*)dest = *(uint64_t*)src; break;
	case CC(F4,U4): *(   float*)dest = *(uint64_t*)src; break;
	case CC(F8,U4): *(  double*)dest = *(uint64_t*)src; break;
#endif//I_CAN_HAS_INT64

#ifdef I_CAN_HAS_LONGDOUBLE
	// to longdouble
	case CC(F6,I8): *(longdouble*)dest = *(  int8_t*)src; break;
	case CC(F6,I6): *(longdouble*)dest = *( int16_t*)src; break;
	case CC(F6,I2): *(longdouble*)dest = *( int32_t*)src; break;
	case CC(F6,U8): *(longdouble*)dest = *( uint8_t*)src; break;
	case CC(F6,U6): *(longdouble*)dest = *(uint16_t*)src; break;
	case CC(F6,U2): *(longdouble*)dest = *(uint32_t*)src; break;
	case CC(F6,F4): *(longdouble*)dest = *(   float*)src; break;
	case CC(F6,F8): *(longdouble*)dest = *(  double*)src; break;

	// from longdouble
	case CC(I8,F6): *(  int8_t*)dest = *(longdouble*)src; break;
	case CC(I6,F6): *( int16_t*)dest = *(longdouble*)src; break;
	case CC(I2,F6): *( int32_t*)dest = *(longdouble*)src; break;
	case CC(U8,F6): *( uint8_t*)dest = T8(0.5+*(longdouble*)src); break;
	case CC(U6,F6): *(uint16_t*)dest = T6(0.5+*(longdouble*)src); break;
	case CC(U2,F6): *(uint32_t*)dest = *(longdouble*)src; break;
	case CC(F4,F6): *(   float*)dest = *(longdouble*)src; break;
	case CC(F8,F6): *(  double*)dest = *(longdouble*)src; break;

#ifdef I_CAN_HAS_INT64 //(nested)
	case CC(F6,I4): *(longdouble*)dest = *( int64_t*)src; break;
	case CC(F6,U4): *(longdouble*)dest = *(uint64_t*)src; break;
	case CC(I4,F6): *( int64_t*)dest = *(longdouble*)src; break;
	case CC(U4,F6): *(uint64_t*)dest = *(longdouble*)src; break;
#endif//I_CAN_HAS_INT64 (nested)

#endif//I_CAN_HAS_LONGDOUBLE

	default: fail("bad conversion from %d to %d", src_fmt, dest_fmt);
	}
}




