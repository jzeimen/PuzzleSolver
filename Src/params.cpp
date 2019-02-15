/*
 * params.cpp
 *
 */

#include "params.h"

#include <sstream>

params::params() {
}

bool params::isSolving() const {
    return solving;
}

void params::setSolving(bool solving) {
    this->solving = solving;
}
    
bool params::isGuidedSolution() const {
    return guidedSolution;
}

void params::setGuidedSolution(bool guidedSolution) {
    this->guidedSolution = guidedSolution;
}

uint params::getEstimatedPieceSize() const {
    return estimatedPieceSize;
}

void params::setEstimatedPieceSize(uint estimatedPieceSize) {
    this->estimatedPieceSize = estimatedPieceSize;
}

std::string params::getPieceOrder() const {
    return pieceOrder;
}

void params::setPieceOrder(std::string pieceOrder) {
    this->pieceOrder = pieceOrder;
}

uint params::getInitialPieceId() const {
    return initialPieceId;
}

void params::setInitialPieceId(uint initialPieceId) {
    this->initialPieceId = initialPieceId;
}
    
void params::setSaveAll(bool saveAll)
{
    this->saveAll = saveAll;
}

bool params::isSavingOriginals() const {
    return savingOriginals || saveAll;
}

void params::setSavingOriginals(bool savingOriginals) {
    this->savingOriginals = savingOriginals;
}

bool params::isSavingBlackWhite() const {
    return savingBlackWhite || saveAll;
}

void params::setSavingBlackWhite(bool savingBlackWhite) {
    this->savingBlackWhite = savingBlackWhite;
}

bool params::isSavingColor() const {
    return savingColor || saveAll;
}

void params::setSavingColor(bool savingColor) {
    this->savingColor = savingColor;
}

bool params::isSavingContours() const {
    return savingContours || saveAll;
}

void params::setSavingContours(bool savingContours) {
    this->savingContours = savingContours;
}

bool params::isSavingCorners() const {
    return savingCorners || saveAll;
}

void params::setSavingCorners(bool savingCorners) {
    this->savingCorners = savingCorners;
}

bool params::isSavingEdges() const {
    return savingEdges || saveAll;
}

void params::setSavingEdges(bool savingEdges) {
    this->savingEdges = savingEdges;
}

bool params::isSavingMatches() const {
    return savingMatches || saveAll;
}

void params::setSavingMatches(bool savingMatches) {
    this->savingMatches = savingMatches;
}

std::string params::getInputDir() const {
    return inputDir;
}

void params::setInputDir(std::string inputDir) {
    if (inputDir.back() != '/') inputDir = inputDir + "/";
    this->inputDir = inputDir;
}

bool params::isVerbose() const {
    return verbose;
}

void params::setVerbose(bool Verbose) {
    verbose = Verbose;
}

std::string params::getOutputDir() const {
    return outputDir;
}

void params::setOutputDir(std::string outputDir) {
    if (outputDir.back() != '/') outputDir = outputDir + "/";
    this->outputDir = outputDir;
}

float params::getPartitionFactor() const {
    return partitionFactor;
}

void params::setPartitionFactor(float partitionFactor) {
    this->partitionFactor = partitionFactor;
}

std::string params::getSolutionFileBasename() const {
    return solutionFileBasename;
}

void params::setSolutionFileBasename(std::string solutionImageFilename) {
    this->solutionFileBasename = solutionImageFilename;
}

uint params::getThreshold() const {
    return threshold;
}

void params::setThreshold(uint threshold) {
    this->threshold = threshold;
}

bool params::isUsingMedianFilter() const {
    return useMedianFilter;
}

void params::setUsingMedianFilter(bool useMedianFilter) {
    this->useMedianFilter = useMedianFilter;
}

uint params::getMedianBlurKSize() const {
    return medianBlurKSize;
}

void params::setMedianBlurKSize(uint medianBlurKSize) {
    this->medianBlurKSize = medianBlurKSize;
}
    
uint params::getFindCornersBlockSize() const {
    return findCornersBlockSize;
}

void params::setFindCornersBlockSize(uint findCornersBlockSize) {
    this->findCornersBlockSize = findCornersBlockSize;
}
    
uint params::getMinCornersQuality() const {
    return minCornersQuality;
}

void params::setMinCornersQuality(uint minCornersQuality) {
    this->minCornersQuality = minCornersQuality;
}

float params::getGuiScale() const {
    return guiScale;
}

void params::setGuiScale(float cornerEditorScale) {
    this->guiScale = cornerEditorScale;
}

bool params::isAdjustingCorners() const {
    return editingCorners;
}

void params::setAdjustingCorners(bool editingCorners) {
    this->editingCorners = editingCorners;
}

float params::getCscoreLimit() const {
    return cscoreLimit;
}

void params::setCscoreLimit(float cscoreLimit) {
    this->cscoreLimit = cscoreLimit;
}

float params::getEscoreLimit() const {
    return escoreLimit;
}

void params::setEscoreLimit(float escoreLimit) {
    this->escoreLimit = escoreLimit;
}

int params::getWorkOnPiece() const {
    return workOnPiece;
}

void params::setWorkOnPiece(int workOnPiece) {
    this->workOnPiece = workOnPiece;
}

bool params::isVerifyingContours() const {
    return verifyingContours;
}

void params::setVerifyingContours(bool verifyingContours) {
    this->verifyingContours = verifyingContours;
}
    
inline const char * const bool_to_string(bool b)
{
  return b ? "true" : "false";
}

std::string params::to_string() const {
    std::stringstream stream;
    stream << "verbose ................ " << bool_to_string(this->isVerbose()) << std::endl;
    stream << "input images dir ....... " << this->getInputDir() << std::endl;
    stream << "output dir ............. " << this->getOutputDir() << std::endl;
    stream << "solution name .......... " << this->getSolutionFileBasename() << std::endl;
    stream << "solve puzzle ........... " << bool_to_string(this->isSolving()) << std::endl;    
    stream << "guided solution mode ... " << bool_to_string(this->isGuidedSolution()) << std::endl;    
    stream << "work on piece .......... " << this->getWorkOnPiece() << std::endl;
    stream << "estimated piece size ... " << this->getEstimatedPieceSize() << std::endl;
    stream << "threshold .............. " << this->getThreshold() << std::endl;
    stream << "median filter .......... " << bool_to_string(this->isUsingMedianFilter()) << std::endl;
    stream << "median blur ksize ...... " << (this->getMedianBlurKSize()) << std::endl;
    stream << "piece order ............ " << this->getPieceOrder() << std::endl;        
    stream << "partition factor ....... " << this->getPartitionFactor() << std::endl;   
    stream << "find corners block size  " << this->getFindCornersBlockSize() << std::endl;
    stream << "min corners quality .... " << this->getMinCornersQuality() << std::endl;
    stream << "adjust corners ......... " << bool_to_string(this->isAdjustingCorners()) << std::endl;          
    stream << "gui scale .............. " << this->getGuiScale() << std::endl;   
    stream << "cscore limit ........... " << this->getCscoreLimit() << std::endl;   
    stream << "escore limit ........... " << this->getEscoreLimit() << std::endl;       
    stream << "verify contours ........ " << bool_to_string(this->isVerifyingContours()) << std::endl;
    stream << "save original images ... " << bool_to_string(this->isSavingOriginals()) << std::endl;
    stream << "save contour images .... " << bool_to_string(this->isSavingContours()) << std::endl;
    stream << "save b&w images ........ " << bool_to_string(this->isSavingBlackWhite()) << std::endl;    
    stream << "save color images ...... " << bool_to_string(this->isSavingColor()) << std::endl;        
    stream << "save corner images ..... " << bool_to_string(this->isSavingCorners()) << std::endl;        
    stream << "save edge images ....... " << bool_to_string(this->isSavingEdges()) << std::endl;      
    stream << "save matched edges ..... " << bool_to_string(this->isSavingMatches()) << std::endl; 
    return stream.str();
}

params::~params() {
}


