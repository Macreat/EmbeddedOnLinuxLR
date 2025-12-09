# WeatherShield Testing Workspace

Esta carpeta almacenará:

- Código de pruebas unitarias/integración (pytest) organizado por módulos (`/unit`, `/integration`).
- Scripts de soporte (`fixtures.py`, simuladores de sensores, generadores de datos).
- Resultados y evidencia (`/results`, `/evidence/<test-id>`), manteniendo trazabilidad con `docs/verification-plan.md`.

## Próximos pasos

1. Crear estructura inicial `tests/unit` y `tests/integration` con `__init__.py`.
2. Configurar `pytest.ini` y `tox.ini` para ejecución local y en CI.
3. Documentar comandos de ejecución en `README` principal (Sección QA).
4. Subir evidencia de pruebas a medida que se ejecuten los IT listados.
