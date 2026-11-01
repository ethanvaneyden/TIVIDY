import type { ScenarioId } from '../types/types';
import { del, get, post } from './client';

export interface ApiItem {
    id: string;
    type: string;
    name: string;
    state: string;
    reviewMethod?: string;
    assignee: string | null;
    reviewer: string | null;
    result?: number;
    diagnosis?: string;
    age?: number;
    bed?: string;
    discharged?: boolean;
}

export interface ApiContainer {
    id: string;
    type: string;
    name: string;
    phase?: string;
    children: ApiItem[];
}

export interface ApiRoot {
    id: string;
    type: string;
    name: string;
    children: ApiContainer[];
}

export interface ActionResult {
    result: string;
}

export interface NewPatient {
    name: string;
    age: number;
    diagnosis: string;
}

export interface NewSubmission {
    studentName: string;
}

export const INSTANCE_DELETE_ENABLED: boolean = import.meta.env.VITE_INSTANCE_DELETE === 'true';

const CONTAINER_ROUTE: Record<ScenarioId, string> = {
    education: 'submit',
    healthcare: 'admit',
};

const ITEM_COLLECTION: Record<ScenarioId, string> = {
    education: 'submissions',
    healthcare: 'patients',
};

function scenarioBase(scenario: ScenarioId): string {
    return `/api/${scenario}`;
}

function instanceBase(scenario: ScenarioId, instanceId: string): string {
    return `${scenarioBase(scenario)}/${encodeURIComponent(instanceId)}`;
}

function containerBase(scenario: ScenarioId, instanceId: string, containerId: string): string {
    return `${instanceBase(scenario, instanceId)}/containers/${encodeURIComponent(containerId)}`;
}

function itemBase(scenario: ScenarioId, instanceId: string, itemId: string): string {
    return `${instanceBase(scenario, instanceId)}/items/${encodeURIComponent(itemId)}`;
}

export function instanceName(instanceId: string): string {
    const name = instanceId.replace(/^(edu|hc)_\d+_/, '');
    return name === '' ? instanceId : name;
}

export function listInstances(scenario: ScenarioId): Promise<string[]> {
    return get<string[]>(`${scenarioBase(scenario)}/instances`);
}

export async function startInstance(scenario: ScenarioId, name: string): Promise<string> {
    const response = await post<{ instanceId: string }>(`${scenarioBase(scenario)}/start`, { name });
    return response.instanceId;
}

export function deleteInstance(scenario: ScenarioId, instanceId: string): Promise<ActionResult> {
    return del<ActionResult>(instanceBase(scenario, instanceId));
}

export function getState(scenario: ScenarioId, instanceId: string): Promise<ApiRoot> {
    return get<ApiRoot>(`${instanceBase(scenario, instanceId)}/state`);
}

export function getHistory(scenario: ScenarioId, instanceId: string): Promise<string[]> {
    return get<string[]>(`${instanceBase(scenario, instanceId)}/history`);
}

export function addContainer(scenario: ScenarioId, instanceId: string, name: string): Promise<ActionResult> {
    return post<ActionResult>(`${instanceBase(scenario, instanceId)}/${CONTAINER_ROUTE[scenario]}`, {
        containerName: name,
    });
}

export function startItem(scenario: ScenarioId, instanceId: string, itemId: string): Promise<ActionResult> {
    return post<ActionResult>(`${itemBase(scenario, instanceId, itemId)}/start`);
}

export function completeItem(
    scenario: ScenarioId,
    instanceId: string,
    itemId: string,
    value: number,
): Promise<ActionResult> {
    return post<ActionResult>(`${itemBase(scenario, instanceId, itemId)}/complete`, { value });
}

export function approveContainer(
    scenario: ScenarioId,
    instanceId: string,
    containerId: string,
): Promise<ActionResult> {
    return post<ActionResult>(`${containerBase(scenario, instanceId, containerId)}/approve`);
}

export function overrideItem(
    scenario: ScenarioId,
    instanceId: string,
    itemId: string,
    value: number,
    accept: boolean,
): Promise<ActionResult> {
    return post<ActionResult>(`${itemBase(scenario, instanceId, itemId)}/override`, {
        value,
        accept: accept ? 1 : 0,
    });
}

export function openContainer(
    scenario: ScenarioId,
    instanceId: string,
    containerId: string,
): Promise<ActionResult> {
    return post<ActionResult>(`${containerBase(scenario, instanceId, containerId)}/open`);
}

export function addItem(
    scenario: ScenarioId,
    instanceId: string,
    containerId: string,
    payload: NewPatient | NewSubmission,
): Promise<ActionResult> {
    return post<ActionResult>(
        `${containerBase(scenario, instanceId, containerId)}/${ITEM_COLLECTION[scenario]}`,
        payload,
    );
}
