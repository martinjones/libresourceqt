#include <QDebug>
#include "commandlineparser.h"

QHash<QString, ResourcePolicy::ResourceType> CommandLineParser::resourceValues;

CommandLineParser::CommandLineParser():
        allResources(), optResources(), autoRelease(false), alwaysReply(false),
        verbose(false), allowUnkownResourceClass(false), output(stdout)
{
    resourceValues["AudioPlayback"] = ResourcePolicy::AudioPlaybackType;
    resourceValues["VideoPlayback"] = ResourcePolicy::VideoPlaybackType;
    resourceValues["AudioRecording"] = ResourcePolicy::AudioRecorderType;
    resourceValues["VideoRecording"] = ResourcePolicy::VideoRecorderType;
    resourceValues["Vibra"] = ResourcePolicy::VibraType;
    resourceValues["Leds"] = ResourcePolicy::LedsType;
    resourceValues["Backlight"] = ResourcePolicy::BacklightType;
    resourceValues["SystemButton"] = ResourcePolicy::SystemButtonType;
    resourceValues["LockButton"] = ResourcePolicy::LockButtonType;
    resourceValues["ScaleButton"] = ResourcePolicy::ScaleButtonType;
    resourceValues["SnapButton"] = ResourcePolicy::SnapButtonType;
    resourceValues["LensCover"] = ResourcePolicy::LensCoverType;
    resourceValues["HeadsetButtons"] = ResourcePolicy::HeadsetButtonsType;
}

CommandLineParser::~CommandLineParser()
{
}

bool CommandLineParser::parseArguments()
{
    QStringList args = QCoreApplication::arguments();
    QStringList::const_iterator ci = args.constBegin();
    // skip the first arg, which is program name
    ++ci;
    while (ci != args.constEnd()) {
        if ((*ci).at(0) == QChar('-')) {
            switch ((*ci).at(1).toAscii()) {
            case 'd':
                ++ci;
            case 'm':
            case 's':
            case 't':
                break;
            case 'f':
                if (!parseModeValues(*(++ci))) {
                    return false;
                }
                break;
            case 'o':
                if (!parseResourceList(*(++ci), optResources)) {
                    output << "Failed to parse resources: " << *ci << endl;
                    return false;
                }
                break;
            case 'u':
                allowUnkownResourceClass = true;
                break;
            case 'h':
            default:
                usage();
                return false;
            }
        }
        else {
            //assume there are no more args
            break;
        }
        ++ci;
    }

    if (ci == args.constEnd()) {
        usage();
        return false;
    }
    else {
        if (!parseClassString(*ci)) {
            return false;
        }
        ++ci;
        if (ci != args.constEnd()) {
            if (!parseResourceList(*ci, allResources)) {
                output << "Error while parsing resource list" << endl;
            }
        }
    }

    if (!allResources.contains(optResources)) {
        output << "optional resources are not subset of all resources" << endl;
        return false;
    }
    return true;
}

bool CommandLineParser::parseClassString(const QString &str)
{
    if (!allowUnkownResourceClass &&
            (str != "call") &&
            (str != "camera") &&
            (str != "ringtone") &&
            (str != "alarm") &&
            (str != "navigator") &&
            (str != "game") &&
            (str != "player") &&
            (str != "event") &&
            (str != "background") &&
            (str != "videoeditor")) {
        output << "invalid class " << str;
        return false;
    }

    applicationClass = str;
    return true;
}

bool CommandLineParser::parseResourceList(const QString &resourceListStr,
        QSet<ResourcePolicy::ResourceType> &resources)
{
    if (resourceListStr.isEmpty() || resourceListStr.isNull()) {
        return false;
    }
    else {
        QStringList resList = resourceListStr.split(",", QString::SkipEmptyParts);

        foreach(QString res, resList) {
            if (resourceValues.contains(res)) {
                resources.insert(resourceValues[res]);
            }
        }
    }

    return true;
}

bool CommandLineParser::parseModeValues(const QString &modeListStr)
{
    if (modeListStr.isEmpty() || modeListStr.isNull()) {
        return false;
    }

    QStringList modeList = modeListStr.split(",", QString::SkipEmptyParts);

    foreach(QString mode, modeList) {
        if (mode == "AutoRelease") {
            autoRelease = true;
        }
        else if (mode == "AlwaysReply") {
            alwaysReply = true;
        }
        else {
            output << "Ignoring unknown mode '" << mode << "'!" << endl;
        }
    }
    return true;
}

void CommandLineParser::usage()
{
    output << "usage: resourceqt-client [-h] [-m mode-values]" <<
    "[-o optional-resources] [-s shared-resources -m shared-mask] " <<
    "class all-resources" << endl;
    output << "\toptions:" << endl;
    output << "\t  h\tprint this help message and exit" << endl;
    output << "\t  f\tmode values. See 'modes' below for the "
    "\n\t\tsyntax of <mode-values>" << endl;
    output << "\t  o\toptional resources. See 'resources' below for the "
    "syntax of\n\t\t<optional-resources>" << endl;
    output << "\tclass:" << endl;
    output << "\t\tcall\t  - for native or 3rd party telephony" << endl;
    output << "\t\tcamera\t  - for photo applications" << endl;
    output << "\t\tringtone  - for ringtones" << endl;
    output << "\t\talarm\t  - for alarm clock" << endl;
    output << "\t\tnavigator - for mapping applications" << endl;
    output << "\t\tgame\t  - for gaming" << endl;
    output << "\t\tplayer\t  - for media playback/recording" << endl;
    output << "\t\tevent\t  - for messaging and other event notifications" << endl;
    output << "\t\tbackground - for thumbnailing etc" << endl;
    output << "\t\tvideoeditor\t  - for video editing/MMS" << endl;
    output << "\tresources:" << endl;
    output << "\t  comma separated list of the following resources" << endl;

    QHash<QString, ResourcePolicy::ResourceType>::const_iterator ci = resourceValues.constBegin();
    while (ci != resourceValues.constEnd()) {
        output << "\t\t" << ci.key() << endl;
        ++ci;
    }
    output << "\t  no whitespace allowed in the resource list." << endl;
    output << "\tmodes:" << endl;
    output << "\t  comma separated list of the following modes" << endl;
    output << "\t\tAutoRelease" << endl;
    output << "\t\tAlwaysReply" << endl;
}

const QSet<ResourcePolicy::ResourceType>& CommandLineParser::resources() const
{
    return allResources;
}

const QSet<ResourcePolicy::ResourceType>& CommandLineParser::optionalResources() const
{
    return optResources;
}

QString CommandLineParser::resourceApplicationClass() const
{
    return applicationClass;
}

bool CommandLineParser::shouldAutoRelease() const
{
    return autoRelease;
}

bool CommandLineParser::shouldAlwaysReply() const
{
    return alwaysReply;
}

bool CommandLineParser::shouldBeVerbose() const
{
    return verbose;
}

