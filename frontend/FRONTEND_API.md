# Frontend and backend contract

Both scenarios now run on live data. `src/api/workflow.ts` is the only place that knows the routes.

## Run

```
npm install
npm run mock-api     # fake backend on :8080, same routes and rules as the C++ server
npm run dev
npm test
```

`VITE_API_URL` changes the server (default `http://localhost:8080`).
`VITE_INSTANCE_DELETE=true` shows the delete buttons once the server has a delete route.

## Routes the UI calls

Existing in `RestServer.cpp`, for both `/api/education` and `/api/healthcare`:

| Call | Used for |
|---|---|
| `GET instances` | instance picker (ids look like `edu_0_Name` and `hc_0_Name`, the name is read from the id) |
| `POST start {name}` | create instance, then open it |
| `GET :id/state` | tree: root, containers, items |
| `GET :id/history` | history panel |
| `POST :id/submit {containerName}` (education), `POST :id/admit {containerName}` (healthcare) | add assignment or ward |
| `POST :id/items/:itemId/start` | start item |
| `POST :id/items/:itemId/complete {value}` | mark (0-100) or severity (0-10) |
| `POST :id/containers/:cid/approve` | batch approval |
| `POST :id/items/:itemId/override {value, accept}` | lecturer or chief resolves a rejected item |

Not in the backend yet (the UI already calls them):

| Call | Body |
|---|---|
| `POST :id/containers/:cid/open` | none. Intake to Active and assign workers (`openWard` / marking start) |
| `POST :id/containers/:cid/patients` | `{name, age, diagnosis}` |
| `POST :id/containers/:cid/submissions` | `{studentName}` |
| `DELETE /api/:scenario/:id` | none |

State JSON additions the UI reads (it copes without them, see below):

- container: `phase`, from `Container::getPhaseName()`
- item: `result`, from `Item::getResult()` when `hasResult()` (mark for education, severity for healthcare)
- healthcare item: `diagnosis`, optionally `age`, `bed`, `discharged`

Without `phase` the UI derives it from item states. That cannot tell an Active container with no workers from Intake.

## What each screen does

- Instance picker: lists, creates (then opens the new instance), deletes when enabled.
- Education and healthcare workspaces: add container, add item (Intake only), open, start, complete, approve batch, resolve rejected.
  Buttons only appear in the phase where the action is legal. Server errors show in a dismissible alert and every action reloads state and history.
- Value rules match the backend: education marks 0-100, rejected below 40, moderator 40-49, TA 50-70, lecturer 71-100;
  healthcare severity 0-10, triage nurse 1-3, admitting doctor 4-6, chief of medicine 7-10, 0 is rejected.
  Resolving: lecturer accepts 0-100, chief accepts 1-10.

## Backend work still needed for a real end-to-end run

1. Every `TividyWorkflow` command (`submitEducation`, `admitPatient`, `startItem`, `completeItem`, `approveContainer`, `overrideItem`) is a scaffold that returns Success without changing anything, so state never moves.
2. `startHealthcare()` builds a `WorkflowInstance` from null pointers, and `EducationWorkflowFactory::createBuilder()` returns `nullptr`.
   Use `HospitalAdmissionBuilder` with `HospitalAdmissionCoordinator`, and `AssignmentMarkingBuilder` with `AssignmentMarkingCoordinator`.
3. Add the four missing routes and the state fields above to `RestServer`, `JsonSerializer` and `TividyWorkflow`.
4. Return participant names instead of ids for assignee and reviewer, and identify the operator on approve and override.

## Tests

`npm test` runs 49 tests: route and payload checks against `RestServer.cpp`, state mapping, input validation,
and rendered screens against a fake server (open, start, complete, approve, resolve, add, create instance, load failure).
