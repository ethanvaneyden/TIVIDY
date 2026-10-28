#pragma once

// Passed to WorkflowMediator::notify after the operation has committed.
namespace LifecycleEvents
{
    const char* const ItemAssigned = "ItemAssigned";
    const char* const ItemStarted = "ItemStarted";
    const char* const ItemCompleted = "ItemCompleted";
    const char* const ItemRejected = "ItemRejected";
    const char* const ItemReviewed = "ItemReviewed";
    const char* const ItemReviewResolved = "ItemReviewResolved";
    const char* const PhaseChanged = "PhaseChanged";
}
