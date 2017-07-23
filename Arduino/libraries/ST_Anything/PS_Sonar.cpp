//******************************************************************************************
//  File: PS_Sonar.cpp
//  Authors: Paul Needler, based on code from Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Sonar is a class which implements the SmartThings "Sensor" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the
//			  pulse width of a MaxSonar sensor and convert it to cm distance, or a % capacity if minRead and tankDepth are set in the SmartThings app.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Sonar sensor1(F("battery1"), 1, 0, PIN_SONAR_1, st::PS_Sonar::PW, st::PS_Sonar::LV, st::PS_Sonar::BEST, 10);
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
//		2017-07-18	Paul Needler	 Modified for MaxSonar Sensor
//
//
//******************************************************************************************

#include "PS_Sonar.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//constructor - called in your sketch's global variable declaration section
	PS_Sonar::PS_Sonar(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalInputPin,  PS_Sonar_INPUT_t input, PS_Sonar_MODEL_t model, PS_Sonar_FILTER_t filter,
	                   uint8_t sample_size):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0),
		input(input),
		model(model),
		filter(filter),
		sample_size(sample_size)

	{
		setPin(digitalInputPin);
		if (st::PollingSensor::debug) {
			Serial.println("Sonar Digital Pin: ");
			Serial.print(digitalInputPin);
			Serial.println();
		}


		#ifdef PS_Sonar_WITH_SOFTWARE_SERIAL

		    if (input == TX) {
		        serial = new RxSoftwareSerial(pin, true);
		        ((RxSoftwareSerial*)serial)->begin(9600);
		    } else {
		#endif

		pinMode(digitalInputPin, INPUT);

		init();

	}

/*
// NOT COMPLETED - I DON'T PLAN TO USE serial	#ifdef PS_Sonar_WITH_SOFTWARE_SERIAL
	PS_Sonar::PS_Sonar(Stream* serial, PS_Sonar_MODEL_t model, PS_Sonar_FILTER_t filter, uint8_t sample_size) :
	    input(TX), model(model), serial(serial), filter(filter), sample_size(sample_size)
	{
	    init();
	}
	#endif

	*/


void PS_Sonar::init()

{
	if (st::PollingSensor::debug) {
		Serial.println("Calling init");
	}
    switch (filter) {
    case MEDIAN:
    case HIGHEST_MODE:
    case LOWEST_MODE:
    case BEST:
        if (sample_size == 0)
            sample_size = 5;
        else if (sample_size % 2)
            sample_size++;

        break;

    case SIMPLE:
        if (sample_size == 0)
            sample_size = 2;

        break;

    case NONE:
    default:
        sample_size = 1;
        break;
    }

    sample = new float[sample_size];
}

	//destructor
	PS_Sonar::~PS_Sonar()
	{
		delete[] sample;
//    delete serial;
	}

	float PS_Sonar::getRange()
	{
	    float range;

	    readSample();

	    switch (filter) {
	    case MEDIAN:
	        range = getSampleMedian();
	        break;

	    case HIGHEST_MODE:
	        range = getSampleMode(true);
	        break;

	    case LOWEST_MODE:
	        range = getSampleMode(false);
	        break;

	    case SIMPLE:
	        while (sample[0] != sample[sample_size - 1])
	            Serial.print("fails to push to Sample");
							pushToSample(readSensor());

	        range = sample[0];
	        break;

	    case BEST:
	        range = getSampleBest();
	        break;

	    case NONE:
	    default:
	        range = sample[0];
	        break;
	    }
			if (st::PollingSensor::debug) {
				Serial.print(range);
				Serial.println();
			}
	    return range;
	}

float PS_Sonar::getSampleMedian()
	{
	    return sample[sample_size / 2];
	}

	float PS_Sonar::getSampleMode(bool highest)
	{
	    float mode = sample[0];
	    uint8_t mode_count = 1;
	    uint8_t count = 1;

	    for (int i = 1; i < sample_size; i++) {
	        if (sample[i] == sample[i - 1])
	            count++;
	        else
	            count = 1;

	        if (sample[i] == mode)
	            mode_count++;
	        else if (!highest && count > mode_count || highest && count == mode_count) {
	            mode_count = count;
	            mode = sample[i];
	        }
	    }

	    return mode;
	}

	float PS_Sonar::getSampleBest()
	{
		if (st::PollingSensor::debug) {
			Serial.print("getSampleBest(): ");
		}
	    float range;

	    if ((range = getSampleMode(true)) != getSampleMode(false))
	        range = getSampleMedian();
	    return range;
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	// Also capture additional parameters from app, minRead and tankDepth and use these to map values to %.
	void PS_Sonar::beSmart(const String &str)
	{
		if (st::PollingSensor::debug) {
			Serial.println("PS_Sonar::beSmart(): ");
			Serial.println(str);
		}
		int ind1 = str.indexOf(' ', str.indexOf(' ') + 1);
		String s = str.substring(str.indexOf(' ') + 1, ind1 + 1);
		int ind2 = str.indexOf(' ', ind1+1);
		String t = str.substring(ind1+1, ind2+1);
		int ind3 = str.indexOf(' ', ind2+1);
		String u = str.substring(ind2+1);

		//Set PollingSensor
		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Sonar::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Sonar::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}

		//Set map for minRead
		if (t.toInt() != 0) {
			minRead = t.toInt();
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Sonar::minRead set to "));
				Serial.println(t.toInt());
			}
		}
		else {
			minRead = 0;
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Sonar::beSmart cannot convert "));
				Serial.print(t);
				Serial.println(F(" to an Integer."));
			}
		}
		//Set map for tankDepth
		if (u.toInt() != 0) {
			tankDepth = u.toInt();
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Sonar::tankDepth set to "));
				Serial.println(u.toInt());
			}
		}
		else {
			tankDepth = 0;
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Sonar::beSmart cannot convert "));
				Serial.print(u);
				Serial.println(F(" to an Integer."));
			}
		}

	}

	//function to get data from sensor and queue results for transfer to ST Cloud
	void PS_Sonar::getData()
	{
	digitalWrite(BUILTIN_LED, HIGH);  // turn on LED with voltage HIGH
	digitalWrite(D1, LOW);  // turn on LED with voltage LOW
	m_nSensorValue = getRange();
	if (st::PollingSensor::debug) {
		Serial.print("Data: ");
		Serial.print(m_nSensorValue);
		Serial.print("cm");
		Serial.println();
	}
	if (minRead > 0) {
		if (tankDepth > 0) {
			//minRead and tankDepth both set - map values
				m_nSensorValue = map(m_nSensorValue, minRead, tankDepth, 100, 0);
				if (m_nSensorValue < 0) { // set value to 0% if greater than tank depth
						m_nSensorValue = 0;
					}
					Everything::sendSmartString(getName() + " " + String(m_nSensorValue) + " %");
					if (st::PollingSensor::debug) {
						Serial.print("Mapping values: ");
						Serial.print("Low: ");
						Serial.print(minRead);
						Serial.print(" High: ");
						Serial.print(tankDepth);
						Serial.println();
						Serial.print("Output: ");
						Serial.print(m_nSensorValue);
						Serial.print("%");
						Serial.println();
						}

		}
	}
	else {
		Serial.println("Mapped values not found, print depth.");
		Everything::sendSmartString(getName() + " " + String(m_nSensorValue) + " cm");
		if (st::PollingSensor::debug) {
			Serial.print("Data: ");
			Serial.print(m_nSensorValue);
			Serial.print("cm");
			Serial.println();
	}
	delay(1000);
}
	digitalWrite(BUILTIN_LED, LOW);  // turn off LED with voltage LOW
	digitalWrite(D1, HIGH);  // turn off LED with voltage HIGH

	}

	//function to read data from sensor
	float PS_Sonar::readSensor()
	{
		float result;

		    switch (input) {
		    case PW:
		        switch (model) {
		        case LV:
								result = toCentimeters(pulseIn(m_nDigitalInputPin, HIGH) / 147.0);
		            break;

		        case XL:
								result = pulseIn(m_nDigitalInputPin, HIGH) / 58.0;
		            break;

		        case HRLV:
		            result = pulseIn(m_nDigitalInputPin, HIGH) / 10.0;
		            break;

		        default:
		            break;
		        }

		        break;

		    case AN:
		        switch (model) {
		        case LV:
		            result = toCentimeters(analogRead(m_nDigitalInputPin) / 2.0);
		            break;

		        case XL:
		            result = analogRead(m_nDigitalInputPin);
		            break;

		        case HRLV:
		            result = analogRead(m_nDigitalInputPin) * 5.0 / 10.0;
		            break;

		        default:
		            break;
		        }

		        break;
/*
		#ifdef PS_Sonar_WITH_SOFTWARE_SERIAL

		    case TX:
		        switch (model) {
		        case LV:
		            result = toCentimeters(readSensorSerial(3));
		            break;

		        case XL:
		            result = readSensorSerial(3);
		            break;

		        case HRLV:
		            result = readSensorSerial(4) / 10.0;
		            break;

		        default:
		            break;
		        }

		        break;
		#endif
		*/

		    default:
		        break;
		    }

					return result;

	}

	void PS_Sonar::setPin(byte pin)
	{
		m_nDigitalInputPin=pin;
	}

	void PS_Sonar::readSample()
	{
			    // read
	    for (int i = 0; i < sample_size; i++) {
	        sample[i] = readSensor();

	        if (input == AN && i != sample_size - 1)
	            delay(ad_sample_delay);
	    }

	    // sort
	    sortSample();
	}

	void PS_Sonar::pushToSample(float value)
	{
	    for (int i = 0; i < sample_size - 1; i++)
	        sample[i] = sample[i + 1];

					sample[sample_size - 1] = value;
	}


	void PS_Sonar::sortSample()
	{
	    for (int i = 1; i < sample_size; i++) {
	        float j = sample[i];
	        int k;

	        for (k = i - 1; (k >= 0) && (j < sample[k]); k--)
	            sample[k + 1] = sample[k];

	        sample[k + 1] = j;
	    }
	}


}
