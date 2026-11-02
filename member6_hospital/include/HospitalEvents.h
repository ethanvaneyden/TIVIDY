/**
 * @file HospitalEvents.h
 * @brief Event type names the hospital coordinator publishes on the EventBus.
 */

#pragma once

namespace hospital
{
namespace events
{

const char* const TREATMENT_ADDED = "hospital.treatment.added";
const char* const TREATMENT_ASSIGNED = "hospital.treatment.assigned";
const char* const TREATMENT_STARTED = "hospital.treatment.started";
const char* const TREATMENT_COMPLETED = "hospital.treatment.completed";
const char* const TREATMENT_REJECTED = "hospital.treatment.rejected";
const char* const TREATMENT_SETTLED = "hospital.treatment.settled";
const char* const ASSIGNMENT_FAILED = "hospital.assignment.failed";
const char* const APPROVAL_RESOLVED = "hospital.approval.resolved";
const char* const BATCH_PROCESSED = "hospital.batch.processed";
const char* const BATCH_IGNORED = "hospital.batch.ignored";
const char* const WARD_PHASE_ADVANCED = "hospital.ward.phase";
const char* const WARD_PUBLISHED = "WardPublished";
const char* const WARD_REJECTED = "hospital.ward.rejected";

}
}
