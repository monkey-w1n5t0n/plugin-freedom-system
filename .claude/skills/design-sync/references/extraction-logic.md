# Parameter Extraction Logic

This pseudocode shows how to extract parameter counts and feature lists from contracts.

## Extraction Patterns

```typescript
// From creative-brief.md
briefParamCount = extractParameterCount(brief);
briefFeatures = extractFeatureKeywords(brief); // ["preset", "bypass", "meter", etc.]

// From parameter-spec.md
specParamCount = countParameters(parameterSpec);
specParamList = extractParameterNames(parameterSpec);

// From mockup YAML (render-spec.yaml)
mockupParamCount = countControlsInLayout(mockup.layout);
mockupFeaturesList = detectUIFeatures(mockup); // ["preset_browser", "meter", etc.]

// Compare
parameterMatch = (briefParamCount == specParamCount == mockupParamCount);
featureMatch = compareFeatureLists(briefFeatures, mockupFeaturesList);
```

## Feature Detection Keywords

**Brief keywords to look for:**
- "preset" / "preset browser" / "preset management"
- "bypass" / "wet/dry" / "mix"
- "meter" / "visualization" / "spectrum"
- "tabs" / "pages" / "sections"
- Parameter names in quotes or caps (e.g., "GAIN", "FREQUENCY")

**Mockup YAML features:**
- `preset_browser` component
- `meter` / `spectrum_analyzer` components
- `tabs` / `multi_page_layout`
- Count of `knob`, `slider`, `button` elements

## Comparison Logic

```typescript
if (parameterMatch && featureMatch) {
  return "NO_DRIFT";
} else if (minorAdditions && coreConceptIntact) {
  return "ACCEPTABLE_EVOLUTION";
} else if (majorExpansion || missingFeatures) {
  return "DRIFT_REQUIRING_ATTENTION";
} else if (contradictsCoreGoals) {
  return "CRITICAL_DRIFT";
}
```
