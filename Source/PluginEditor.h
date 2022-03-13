/*
  ==============================================================================

    Author:  Chapa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EnvelopeButton.h"

//==============================================================================
/**
*/
class ChapaGranulatorAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener, 
                                             public juce::Slider::Listener, private juce::ChangeListener
{
public:
    ChapaGranulatorAudioProcessorEditor (ChapaGranulatorAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~ChapaGranulatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    virtual void sliderValueChanged(juce::Slider* slider) override;
    virtual void buttonClicked(juce::Button* button) override;
    virtual void buttonStateChanged(juce::Button* button) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void thumbnailChanged();
    void openButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChapaGranulatorAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::MidiKeyboardComponent keyboardComponent;

    EnvelopeButton envelopeButtons[4];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> envelopeButtonsAttachments[4];

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;

    

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChapaGranulatorAudioProcessorEditor)
};