#pragma once
#include <vector>
#include "util.hpp"
#include <jansson.h>


namespace rack {


struct Param {
	float value = 0.0;
};

struct Light {
	/** The square of the brightness value */
	float value = 0.0;
	float getBrightness();
	void setBrightness(float brightness) {
		value = (brightness > 0.f) ? brightness * brightness : 0.f;
	}
	void setBrightnessSmooth(float brightness);
};

struct Input {
	/** Voltage of the port, zero if not plugged in. Read-only by Module */
	float value = 0.0;
	/** Whether a wire is plugged in */
	bool active = false;
	Light plugLights[2];
	/** Returns the value if a wire is plugged in, otherwise returns the given default value */
	float normalize(float normalValue) {
		return active ? value : normalValue;
	}
};

struct Output {
	/** Voltage of the port. Write-only by Module */
	float value = 0.0;
	/** Whether a wire is plugged in */
	bool active = false;
	Light plugLights[2];
};


struct Module {
	std::vector<Param> params;
	std::vector<Input> inputs;
	std::vector<Output> outputs;
	std::vector<Light> lights;
	/** For CPU usage meter */
	float cpuTime = 0.0;

	/** Deprecated, use constructor below this one */
	Module() DEPRECATED {}
	/** Constructs Module with a fixed number of params, inputs, and outputs */
	Module(int numParams, int numInputs, int numOutputs, int numLights = 0) {
		params.resize(numParams);
		inputs.resize(numInputs);
		outputs.resize(numOutputs);
		lights.resize(numLights);
	}
	virtual ~Module() {}

	/** Advances the module by 1 audio frame with duration 1.0 / gSampleRate */
	virtual void step() {}
	virtual void onSampleRateChange() {}

	/** Called when module is created by the Add Module popup, cloning, or when loading a patch or autosave */
	virtual void onCreate() {}
	/** Called when user explicitly deletes the module, not when Rack is closed or a new patch is loaded */
	virtual void onDelete() {}
	/** Called when user clicks Initialize in the module context menu */
	virtual void onReset() {
		// Call deprecated method
		reset();
	}
	/** Called when user clicks Randomize in the module context menu */
	virtual void onRandomize() {
		// Call deprecated method
		randomize();
	}

	/** Override these to store extra internal data in the "data" property */
	virtual json_t *toJson() { return NULL; }
	virtual void fromJson(json_t *root) {}

	/** Deprecated */
	virtual void reset() {}
	/** Deprecated */
	virtual void randomize() {}
};

struct Wire {
	Module *outputModule = NULL;
	int outputId;
	Module *inputModule = NULL;
	int inputId;
	void step();
};

void engineInit();
void engineDestroy();
/** Launches engine thread */
void engineStart();
void engineStop();
/** Does not transfer pointer ownership */
void engineAddModule(Module *module);
void engineRemoveModule(Module *module);
/** Does not transfer pointer ownership */
void engineAddWire(Wire *wire);
void engineRemoveWire(Wire *wire);
void engineSetParam(Module *module, int paramId, float value);
void engineSetParamSmooth(Module *module, int paramId, float value);
void engineSetSampleRate(float sampleRate);
float engineGetSampleRate();
/** Returns the inverse of the current sample rate */
float engineGetSampleTime();

extern bool gPaused;


} // namespace rack
