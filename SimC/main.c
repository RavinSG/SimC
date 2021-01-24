#include <stdio.h>
#include <math.h>
#include <process.h>
#include "lcgrand.h"

#define Q_LIMIT 100
#define BUSY 1
#define IDLE 0

int next_event_type, num_customers_delayed, num_delays_required, num_events, num_in_q, server_status;
float area_num_in_q, area_server_status, mean_interarrival, mean_service, sim_time, time_arrival[
        Q_LIMIT + 1], time_last_event, time_next_event[3], total_of_delay;
FILE *infile, *outfile;

void initialize(void);

void timing(void);

void arrival(void);

void depart(void);

void report(void);

void update_time_avg_stats(void);

float exponent(float mean);

int main(int argc, char *argv[]) {
    infile = fopen("mm1.in", "r");
    outfile = fopen("mm1.out", "w");

    num_events = 2;

    // Read input parameters
    fscanf(infile, "%f %f %d", &mean_interarrival, &mean_service, &num_delays_required);

    // Write report heading and input parameters.
    fprintf(outfile, "Single server queuing system\n\n");
    fprintf(outfile, "Mean interarrival time %11.3f minutes\n\n", mean_interarrival);
    fprintf(outfile, "Mean service time %16.3f minutes\n\n", mean_service);
    fprintf(outfile, "Number of customers %14d\n\n", num_delays_required);

    initialize();

    while (num_customers_delayed < num_delays_required) {
        // Determine the next event
        timing();

        // Update time-average statistics calculator
        update_time_avg_stats();

        switch (next_event_type) {
            case 1:
                arrival();
                break;
            case 2:
                depart();
                break;
            default:
                break;
        }
    }

    // Generate report and end simulation
    report();
    fclose(infile);
    fclose(outfile);

    return 0;
}

void initialize(void) {
    sim_time = 0;

    // Initialize state variable
    server_status = IDLE;
    num_in_q = 0;
    time_last_event = 0;

    // Initialize statistics counter
    num_customers_delayed = 0;
    total_of_delay = 0;
    area_num_in_q = 0;
    area_server_status = 0;

    // Initialize event list. Since no customers are present the departures event is not considered.
    time_next_event[1] = sim_time + exponent(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}

void timing(void) {
    int i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    // Determine the next event type
    for (i = 1; i < num_events; i++) {
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }

    if (next_event_type==0){
        // The event list is empty, stop the simulation.
        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    // Advance the simulation clock to the next event
    sim_time = min_time_next_event;
}