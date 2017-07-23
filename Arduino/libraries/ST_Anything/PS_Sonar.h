//******************************************************************************************
//  File: PS_Sonar.h
//  Authors: Paul Needler (based on script by Dan G Ogorchock & Daniel J Ogorchock (Father and Son))
//
//  Summary:  PS_Voltage is a class which implements the SmartThings "Voltage Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the
//			  voltage on an anlog input pin and then scale it to engineering units.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Voltage sensor1("voltage", 120, 0, PIN_VOLTAGE, 0, 1023, 0, 5);
//
//			  st::PS_Sonar() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- PS_Sonar_INPUT_t - REQUIRED - input type, PW, AN, TX
//				- PS_Sonar_MODEL_t - REQUIRED - LV, XL, HRLV
//				- PS_Sonar_FILTER_t - REQUIRED - Best, Mode, Median, Simple
//				- sample_size - REQUIRED, number of samples for range
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to
//			  the ST Shield.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-04-19  Dan & Daniel   Original Creation
//		2017-07-21	Paul Needler	 Modified for MaxSonar sensor
//
//
//******************************************************************************************

#ifndef ST_PS_SONAR_H
#define ST_PS_SONAR_H

#include "PollingSensor.h"
#include <Arduino.h>


namespace st
{
	class PS_Sonar: public PollingSensor
	{
private:
		uint8_t pin;
		byte m_nDigitalInputPin;		//digital pin connected to the water sensor
		int m_nSensorValue;			//current sensor value
		uint8_t sample_size;
		uint8_t ad_sample_delay;

public:
				// models
				typedef enum {
						LV,
						XL,
						HRLV
				}
				PS_Sonar_MODEL_t;

				// inputs
				typedef enum {
						PW,
						AN,
		#ifdef PS_Sonar_WITH_SOFTWARE_SERIAL
						TX
		#endif

				}
				PS_Sonar_INPUT_t;

				// filters
				typedef enum {
						NONE,
						BEST,
						MEDIAN,
						HIGHEST_MODE,
						LOWEST_MODE,
						SIMPLE
				}
				PS_Sonar_FILTER_t;

			//constructor - called in your sketch's global variable declaration section
			 PS_Sonar(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalInputPin, PS_Sonar_INPUT_t input, PS_Sonar_MODEL_t model, PS_Sonar_FILTER_t filter = NONE,
				 	             uint8_t sample_size = 0);

			//destructor
			virtual ~PS_Sonar();

			// simple api
			float getRange();
			uint8_t getSampleSize()
			{
					return sample_size;
			};

			// advanced api
			void readSample();
			float* getSample()
			{
					return sample;
			};
			float getSampleMedian();
			float getSampleMode(bool highest = true);
			float getSampleBest();

			// getters
			PS_Sonar_MODEL_t getModel()
			{
					return model;
			}
			PS_Sonar_INPUT_t getInput()
			{
					return input;
			}
			PS_Sonar_FILTER_t getFilter()
			{
					return filter;
			}

			// setters
			void setADSampleDelay(uint8_t delay)
			{
					ad_sample_delay = delay;
			}

			// utilities
			static float toCentimeters(float inches)
			{
					return 2.54 * inches;
			};
			static float toInches(float centimeters)
			{
					return centimeters / 2.54;
			};

private:
				// config variables
				PS_Sonar_INPUT_t input;
				PS_Sonar_MODEL_t model;
				PS_Sonar_FILTER_t filter;


			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud
			virtual void getData();

			// core
			float* sample;
			void init();
			float readSensor();
			void pushToSample(float value);
			void sortSample();
			int tankDepth;
			int minRead;

				//sets
			void setPin(byte pin);

};
}
#endif
