#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetCurve( struct inode *inode,struct dda0816_curve curve )
{

			extern struct dda0816_kernel dda0816_resource[16];
			int minor = MINOR(inode->i_rdev);
			struct dda0816_curve curve_copy;

			if (curve.size > DDA08_MAXPOINTS) {
				return -EIO;
			}
			if (curve.points == NULL) {
				return -EIO;
			}
			if (dda0816_resource[minor].curve.size != curve.size && dda0816_resource[minor].curve.points) {
				vfree(dda0816_resource[minor].curve.points);
				dda0816_resource[minor].curve.points = NULL;
			}
			if (dda0816_resource[minor].curve.points == NULL) {

				dda0816_resource[minor].curve.points = (int *)vmalloc(curve.size*sizeof(int));
				if (!dda0816_resource[minor].curve.points) {
					return -ENOMEM;
				}
			} 
			dda0816_resource[minor].curve.size = curve.size;
			dda0816_resource[minor].curve.SoftScale = curve.SoftScale;
			dda0816_resource[minor].scaler=curve.SoftScale;
			dda0816_resource[minor].step=0;
			copy_from_user(dda0816_resource[minor].curve.points, curve.points, curve.size * sizeof(int));
			return(0);
		
}
