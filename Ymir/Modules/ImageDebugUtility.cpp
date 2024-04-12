#include "ImageDebugUtility.hpp"

#include <opencv4/opencv2/opencv.hpp>

#define PI 3.14159265f

namespace RoboPioneers::Modules
{
	/// 绘制正方形
	void ImageDebugUtility::DrawRectangle(cv::Mat& canvas,
	                                      const cv::Point& center, const cv::Size& size,
	                                      const cv::Scalar& color, int thickness)
	{
		cv::line(canvas,
		         cv::Point(center.x - size.width / 2, center.y - size.height / 2),
		         cv::Point(center.x - size.width / 2, center.y + size.height / 2),
		         color, thickness);
		cv::line(canvas,
		         cv::Point(center.x - size.width / 2, center.y + size.height / 2),
		         cv::Point(center.x + size.width / 2, center.y + size.height / 2),
		         color, thickness);
		cv::line(canvas,
		         cv::Point(center.x + size.width / 2, center.y + size.height / 2),
		         cv::Point(center.x + size.width / 2, center.y - size.height / 2),
		         color, thickness);
		cv::line(canvas,
		         cv::Point(center.x + size.width / 2, center.y - size.height / 2),
		         cv::Point(center.x - size.width / 2, center.y - size.height / 2),
		         color, thickness);
	}

	/// 绘制旋转矩形
	void ImageDebugUtility::DrawRotatedRectangle(cv::Mat& canvas,
	                                             const cv::Point &center, const cv::Size &size, float angle,
	                                             const cv::Scalar &color, int thickness)
	{
		ImageDebugUtility::DrawRotatedRectangle(canvas, cv::RotatedRect(center, size, angle),
		                                        color, thickness);
	}

	/// 绘制旋转矩形
	void ImageDebugUtility::DrawRotatedRectangle(
			cv::Mat &canvas, const cv::RotatedRect& rotated_rect,
			const cv::Scalar &color, int thickness)
	{
		cv::Point2f vertices[4];

		rotated_rect.points(vertices);

		for(int i=0;i<4;i++)
		{
			line(canvas, vertices[i], vertices[(i + 1) % 4], color, thickness);
		}
	}
}