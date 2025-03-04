
#include <slide-replace-task.hpp>
#include <tools.hpp>
#include <worker.hpp>

TaskRegistrar<SlideReplaceTask> slideReplaceFactory("slideReplace");

void SlideReplaceTask::setCommandLine(int argc, const char **argv) {
    const cv::String keys =
    "{help h                   |                      | show this message                                   }"
    "{searchImage s            |                      | image that will be searched in the video to replace }"
    "{replacingImage r         |                      | mage that will be replaced in the video             }"
    "{treshold t               |                      | mage that will be replaced in the video             }"
    ;
    
    cv::CommandLineParser parser(argc,argv,keys);
    
    if (!parser.has("searchImage")) {
        parser.printMessage();
        throw std::invalid_argument("searchImage parameter is required");
    }
    
    if (!parser.has("replacingImage")) {
        parser.printMessage();
        throw std::invalid_argument("replacingImage parameter is required");
    }
    
    std::string searchImagePath = parser.get<cv::String>("searchImage");
    std::string replacingImagePath = parser.get<cv::String>("replacingImage");
    
    _searchImage = cv::imread(searchImagePath, cv::IMREAD_COLOR);
    if (_searchImage.empty()) {
        throw std::ios_base::failure("No such image with name " + searchImagePath);
    }
    
    _replacingImage = cv::imread(replacingImagePath, cv::IMREAD_COLOR);
    if (_replacingImage.empty()) {
        throw std::ios_base::failure("No such image with name " + replacingImagePath);
    }
    
    _treshold = 200.0f;
    if (parser.has("treshold")) {
        _treshold = parser.get<int>("treshold");
    }
    
    std::cout << "Frames per second: " << worker()->videoData.fps() <<
    ", total frames: " << worker()->videoData.frameCount() <<
    ", frame width: " << worker()->videoData.width() <<
    ", frame height: " << worker()->videoData.height() << std::endl;
}

void SlideReplaceTask::execute(const cv::Mat & srcImage, cv::Mat & dstImage, uint32_t frameIndex, std::mutex &mutex, uint32_t passIndex) {
    auto similarity = tools::imageSimilarity(srcImage, _searchImage);
    
    if (similarity<=_treshold) {
        cv::Mat difference;
        cv::absdiff(srcImage, _searchImage, difference);
        dstImage = cv::Mat::zeros(difference.rows, difference.cols, CV_8UC3);
        tools::combineImages(srcImage, difference, _replacingImage, _treshold, dstImage);
    }
    else {
        dstImage = srcImage;
    }
}
