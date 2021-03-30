/*
 * params.hpp
 *
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include <string>
#include <sys/types.h>

class params {
private:
    bool solving;
    bool guidedSolution;
    bool verbose;
    std::string inputDir;
    std::string outputDir;
    std::string solutionFileBasename;
    std::string pieceOrder;
    uint initialPieceId;
    uint estimatedPieceSize;
    uint threshold;
    bool useMedianFilter;
    uint medianBlurKSize;
    float partitionFactor;
    uint minCornersQuality;
    bool saveAll;
    bool savingOriginals;
    bool savingContours;
    bool savingBlackWhite;
    bool savingColor;
    bool savingCorners;
    bool savingEdges;
    bool savingMatches;
    uint findCornersBlockSize;
    bool editingCorners;
    float guiScale;
    float cscoreLimit;
    float escoreLimit;
    int workOnPiece;
    bool verifyingContours;

public:
    params();

    bool isSolving() const;

    void setSolving(bool solving);
    
    bool isGuidedSolution() const;

    void setGuidedSolution(bool guidedSolution);

    bool isVerbose() const;

    void setVerbose(bool Verbose);    

    std::string getInputDir() const;

    void setInputDir(std::string inputDir);
    
    std::string getOutputDir() const;

    void setOutputDir(std::string outputDir);
    
    std::string getSolutionFileBasename() const;

    void setSolutionFileBasename(std::string solutionImageFilename);
    
    uint getInitialPieceId() const;

    void setInitialPieceId(uint initialPieceId);
    
    std::string getPieceOrder() const;

    void setPieceOrder(std::string pieceOrder);
    
    uint getEstimatedPieceSize() const;

    void setEstimatedPieceSize(uint estimatedPieceSize);
    
    float getPartitionFactor() const;

    void setPartitionFactor(float partitionFactor);

    uint getThreshold() const;

    void setThreshold(uint threshold);

    bool isUsingMedianFilter() const;

    void setUsingMedianFilter(bool useMedianFilter);
    
    uint getMedianBlurKSize() const;

    void setMedianBlurKSize(uint medianBlurKSize);

    uint getMinCornersQuality() const;

    void setMinCornersQuality(uint minCornersQuality);

    void setSaveAll(bool writeAll);
    
    bool isSavingOriginals() const;

    void setSavingOriginals(bool savingOriginals);

    bool isSavingBlackWhite() const;

    void setSavingBlackWhite(bool savingBlackWhite);

    bool isSavingColor() const;

    void setSavingColor(bool savingColor);

    bool isSavingContours() const;

    void setSavingContours(bool savingContours);

    bool isSavingCorners() const;

    void setSavingCorners(bool savingCorners);

    bool isSavingEdges() const;

    void setSavingEdges(bool savingEdges);    
    
    bool isSavingMatches() const;
    
    void setSavingMatches(bool savingMatches);

    uint getFindCornersBlockSize() const;

    void setFindCornersBlockSize(uint findCornersBlockSize);
    
    float getGuiScale() const;

    void setGuiScale(float cornerEditorScale);

    bool isAdjustingCorners() const;

    void setAdjustingCorners(bool editingCorners);
    
    float getCscoreLimit() const;

    void setCscoreLimit(float cscoreLimit);

    float getEscoreLimit() const;

    void setEscoreLimit(float escoreLimit);
    
    int getWorkOnPiece() const;

    void setWorkOnPiece(int workOnPiece);
    
    bool isVerifyingContours() const;

    void setVerifyingContours(bool verifyingContours);
    
    std::string to_string() const;

    virtual ~params();



};

#endif /* PARAMS_H_ */
