/*
  ==============================================================================

    Author:  Chapa

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChapaGranulatorAudioProcessorEditor::ChapaGranulatorAudioProcessorEditor (ChapaGranulatorAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts), 
      keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      thumbnailCache(5), thumbnail(512, audioProcessor.formatManager, thumbnailCache)
{
    setLookAndFeel(&chapaGranulatorLook);
    //setResizable(true, true);

    thumbnail.addChangeListener(this);

    // Set parameters bounds, id and name
    juce::StringArray slidersText = juce::StringArray("transpose", "density", "position", "length", "panning", "level",
                                                      "r transpose", "r density", "r position", "r length", "r panning", "r level",
                                                      "i transpose", "i density", "i position", "i length", "i panning", "i level");
    juce::StringArray slidersSuffix = juce::StringArray(" cts", "x", "x", " ms", "x", "x", "%", "%", "%", "%", "%", "%", " ms", " ms", " ms", " ms", " ms", " ms");

    // Set envelope buttons
    for (int i = 0; i < audioProcessor.envelopeId.size(); ++i)
    {
        envelopeButtonAttachments[i].reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, audioProcessor.envelopeId[i], envelopeButtons[i]));
        envelopeButtons[i].setButton(i);
        envelopeButtons[i].setClickingTogglesState(true);
        envelopeButtons[i].setRadioGroupId(1, juce::NotificationType::sendNotification);
        envelopeButtons[i].addListener(this);
        envelopeButtons[i].setBounds(10 + (i % 3) * 60, 40 + int(i / 3) * 60, 50, 50);
        addAndMakeVisible(&envelopeButtons[i]);
    }

    // Set parameters sliders
    for (int i = 0; i < audioProcessor.slidersId.size(); ++i)
    {
        sliderAttachments[i].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, audioProcessor.slidersId[i], parameterSliders[i]));
        parameterSliders[i].setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        parameterLabels[i].setText(slidersText[i], juce::NotificationType::dontSendNotification);
        parameterLabels[i].attachToComponent(&(parameterSliders[i]), false);
        parameterSliders[i].setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 30);
        parameterSliders[i].setTextBoxIsEditable(false);
        parameterSliders[i].setTextValueSuffix(slidersSuffix[i]);
        parameterSliders[i].addListener(this);
        parameterSliders[i].setBounds(200 + (i % 6) * 100, 40 + int(i / 6) * 140, 100, 100);
        addAndMakeVisible(&parameterSliders[i]);
    }

    // Set inertia buttons
    for (int i = 0; i < audioProcessor.inertiaId.size(); ++i)
    {
        inertiaButtonAttachments[i].reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, audioProcessor.inertiaId[i], inertiaButtons[i]));
        inertiaButtons[i].setButton(i % 3);
        inertiaButtons[i].setClickingTogglesState(true);
        inertiaButtons[i].setRadioGroupId(int(i / 3) + 1, juce::NotificationType::sendNotification);
        inertiaButtons[i].addListener(this);
        inertiaButtons[i].setBounds(200 + i * 30 + int(i / 3) * 10, 460, 30, 30);
        addAndMakeVisible(&inertiaButtons[i]);
    }

    // Set open file button
    openButton.setButtonText("Open...");
    openButton.addListener(this);
    openButton.setBounds(10, 520, 780, 20);
    addAndMakeVisible(&openButton);

    // Set play button
    playButton.setButtonText("Play");
    playButton.setToggleState(false, juce::dontSendNotification);
    playButton.addListener(this);
    playButton.setBounds(10, 400, 100, 20);
    addAndMakeVisible(&playButton);
    
    // Set GUI size
    setSize (800, 800);
}

ChapaGranulatorAudioProcessorEditor::~ChapaGranulatorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void ChapaGranulatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    juce::Rectangle<int> thumbnailBounds(10, 530, getWidth() - 20, 250);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void ChapaGranulatorAudioProcessorEditor::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::transparentBlack);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::darkorange);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void ChapaGranulatorAudioProcessorEditor::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::transparentBlack);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::darkorange);

    thumbnail.drawChannel(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 0, 1.0f);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1, 1.0f);
}

void ChapaGranulatorAudioProcessorEditor::thumbnailChanged()
{
    repaint();
}

void ChapaGranulatorAudioProcessorEditor::resized()
{
}

//==============================================================================

void ChapaGranulatorAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.updateValue();
}

void ChapaGranulatorAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    button->repaint();
    if (button == &openButton) openButtonClicked();
    if (button == &playButton)
    {
        playButton.setToggleState(!playButton.getToggleState(), juce::dontSendNotification);
        audioProcessor.play = !audioProcessor.play;
    }
}

void ChapaGranulatorAudioProcessorEditor::buttonStateChanged(juce::Button* button)
{
    audioProcessor.updateValue();
}

void ChapaGranulatorAudioProcessorEditor::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File{}, "*.wav", "*.aif", "*.aiff");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                juce::String path(file.getFullPathName());
                audioProcessor.filePath = path;
                thumbnail.setSource(new juce::FileInputSource(file));
                audioProcessor.updateFile();
            }
        });

    audioProcessor.updateValue();
}


void ChapaGranulatorAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail) thumbnailChanged();
}

//==============================================================================
