#!/usr/bin/env python3
"""
Конвертер диалогов из Markdown в JSON для UE5 Dialogue System

Usage:
    python dialogue_converter.py input.md output.json
    python dialogue_converter.py --batch dialogs/ output/
"""

import json
import re
import sys
import argparse
from pathlib import Path
from dataclasses import dataclass, field, asdict
from typing import List, Dict, Optional, Any


@dataclass
class DialogueOption:
    text: str
    next_node: str
    condition: Optional[str] = None
    effects: List[str] = field(default_factory=list)
    hidden: bool = False


@dataclass
class MetaHumanSettings:
    facial_expression: str = "neutral"
    gesture: Optional[str] = None
    look_at: Optional[str] = "player"
    voice_line: Optional[str] = None
    camera_shot: Optional[str] = None
    blend_shapes: Dict[str, float] = field(default_factory=dict)


@dataclass
class DialogueNode:
    id: str
    speaker: str
    text: str
    meta_human: MetaHumanSettings = field(default_factory=MetaHumanSettings)
    options: List[DialogueOption] = field(default_factory=list)
    timeout: Optional[Dict[str, Any]] = None
    auto_advance: bool = False
    next_node: Optional[str] = None


@dataclass
class Dialogue:
    dialogue_id: str
    version: str = "1.0"
    meta: Dict[str, Any] = field(default_factory=dict)
    entry_point: str = ""
    nodes: Dict[str, DialogueNode] = field(default_factory=dict)
    variables: Dict[str, Any] = field(default_factory=dict)


class DialogueParser:
    """Парсер Markdown диалогов"""
    
    # Регулярные выражения
    HEADER_PATTERN = re.compile(r'^---\s*$(.*?)^---\s*$', re.MULTILINE | re.DOTALL)
    NODE_PATTERN = re.compile(r'^##\s+(\w+)\s*$', re.MULTILINE)
    SPEAKER_PATTERN = re.compile(r'^\*\*@(\w+)\*\*\s*(?:\*\(([^)]+)\)\*\s*)?\s*$', re.MULTILINE)
    TEXT_PATTERN = re.compile(r'^\>\s*(.+)$', re.MULTILINE)
    OPTION_PATTERN = re.compile(
        r'^-\s+\[([^\]]+)\]\s*'  # Текст опции
        r'(?:\(([^)]+)\)\s*)?'   # Условие в скобках
        r'\(?#?\s*(\w+)\)?'      # Ссылка на ноду
        r'(?:\s*→\s*([^\n]+))?'  # Эффекты
        r'$',
        re.MULTILINE
    )
    META_HUMAN_INLINE_PATTERN = re.compile(r'\*\(([^)]+)\)\*')
    
    def __init__(self):
        self.dialogue = Dialogue(dialogue_id="", entry_point="")
    
    def parse(self, content: str) -> Dialogue:
        """Основной метод парсинга"""
        
        # Парсим фронтматтер (YAML-like заголовок)
        self._parse_frontmatter(content)
        
        # Убираем фронтматтер для дальнейшего парсинга
        content_without_frontmatter = self.HEADER_PATTERN.sub('', content, count=1).strip()
        
        # Парсим ноды
        self._parse_nodes(content_without_frontmatter)
        
        return self.dialogue
    
    def _parse_frontmatter(self, content: str):
        """Парсит метаданные из фронтматтера"""
        match = self.HEADER_PATTERN.search(content)
        if match:
            frontmatter = match.group(1)
            for line in frontmatter.strip().split('\n'):
                if ':' in line:
                    key, value = line.split(':', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    if key == 'dialogue_id':
                        self.dialogue.dialogue_id = value
                    elif key == 'participants':
                        # Парсим список
                        value = value.strip('[]')
                        self.dialogue.meta['participants'] = [
                            p.strip() for p in value.split(',')
                        ]
                    elif key == 'location':
                        self.dialogue.meta['location'] = value
                    elif key == 'time_of_day':
                        self.dialogue.meta['time_of_day'] = value
                    else:
                        self.dialogue.meta[key] = value
        
        if not self.dialogue.dialogue_id:
            raise ValueError("dialogue_id is required in frontmatter")
    
    def _parse_nodes(self, content: str):
        """Парсит ноды диалога"""
        # Разбиваем на секции по ## node_id
        parts = self.NODE_PATTERN.split(content)
        
        if len(parts) < 2:
            raise ValueError("No dialogue nodes found")
        
        # Первая нода - точка входа
        self.dialogue.entry_point = parts[1]
        
        for i in range(1, len(parts), 2):
            if i + 1 < len(parts):
                node_id = parts[i].strip()
                node_content = parts[i + 1]
                self._parse_node(node_id, node_content)
    
    def _parse_node(self, node_id: str, content: str):
        """Парсит отдельную ноду"""
        lines = content.strip().split('\n')
        
        speaker = "unknown"
        text_lines = []
        options = []
        meta_human = MetaHumanSettings()
        
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            # Спикер и MetaHuman выражение
            speaker_match = self.SPEAKER_PATTERN.match(line)
            if speaker_match:
                speaker = speaker_match.group(1)
                if speaker_match.group(2):
                    # Парсим выражения: emotion, gesture, etc.
                    expressions = [e.strip() for e in speaker_match.group(2).split(',')]
                    for expr in expressions:
                        if expr in self._get_emotion_list():
                            meta_human.facial_expression = expr
                        elif expr.startswith('gesture_'):
                            meta_human.gesture = expr
                        elif expr.startswith('vo_'):
                            meta_human.voice_line = expr
                        elif expr in ['close_up', 'medium_shot', 'wide_shot']:
                            meta_human.camera_shot = expr
                i += 1
                continue
            
            # Текст реплики
            text_match = self.TEXT_PATTERN.match(line)
            if text_match:
                text_lines.append(text_match.group(1))
                i += 1
                continue
            
            # Опции
            if line.startswith('- ['):
                option = self._parse_option_line(line)
                if option:
                    options.append(option)
                i += 1
                continue
            
            i += 1
        
        node = DialogueNode(
            id=node_id,
            speaker=speaker,
            text=' '.join(text_lines),
            meta_human=meta_human,
            options=options
        )
        
        self.dialogue.nodes[node_id] = node
    
    def _parse_option_line(self, line: str) -> Optional[DialogueOption]:
        """Парсит строку опции"""
        # Упрощённый парсинг
        # Пример: - [Текст опции](#next_node) → effect1, effect2
        
        match = re.match(
            r'-\s*\[([^\]]+)\]'           # Текст
            r'(?:\s*\[\[([^\]]+)\]\])?'  # Условие в двойных скобках
            r'\s*\(?#?\s*(\w+)\)?'       # Ссылка
            r'(?:\s*→\s*(.+))?',         # Эффекты
            line
        )
        
        if match:
            text = match.group(1).strip()
            condition = match.group(2)
            next_node = match.group(3)
            effects_str = match.group(4)
            
            effects = []
            if effects_str:
                effects = [e.strip() for e in effects_str.split(',')]
            
            return DialogueOption(
                text=text,
                next_node=next_node,
                condition=condition,
                effects=effects
            )
        
        return None
    
    def _get_emotion_list(self) -> List[str]:
        """Список валидных эмоций"""
        return [
            'neutral', 'happy', 'sad', 'angry', 'surprised',
            'disgusted', 'fearful', 'curious', 'intrigued',
            'cold', 'warm', 'suspicious', 'shocked', 'serious',
            'sympathetic', 'disappointed', 'tired'
        ]


class DialogueConverter:
    """Конвертер диалогов"""
    
    def __init__(self):
        self.parser = DialogueParser()
    
    def convert(self, input_path: Path, output_path: Path):
        """Конвертирует один файл"""
        print(f"Converting: {input_path} -> {output_path}")
        
        content = input_path.read_text(encoding='utf-8')
        dialogue = self.parser.parse(content)
        
        json_data = self._to_json(dialogue)
        
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            json.dumps(json_data, indent=2, ensure_ascii=False),
            encoding='utf-8'
        )
        
        print(f"  ✓ Generated {len(dialogue.nodes)} nodes")
    
    def convert_batch(self, input_dir: Path, output_dir: Path):
        """Конвертирует все .md файлы в директории"""
        for md_file in input_dir.glob('**/*.md'):
            relative = md_file.relative_to(input_dir)
            json_file = output_dir / relative.with_suffix('.json')
            self.convert(md_file, json_file)
    
    def _to_json(self, dialogue: Dialogue) -> dict:
        """Конвертирует Dialogue dataclass в JSON-совместимый dict"""
        
        nodes_json = {}
        for node_id, node in dialogue.nodes.items():
            nodes_json[node_id] = {
                "speaker": node.speaker,
                "text": node.text,
                "meta_human": {
                    "facial_expression": node.meta_human.facial_expression,
                    "gesture": node.meta_human.gesture,
                    "look_at": node.meta_human.look_at,
                    "voice_line": node.meta_human.voice_line,
                    "camera_shot": node.meta_human.camera_shot,
                    "blend_shapes": node.meta_human.blend_shapes
                },
                "options": [
                    {
                        "text": opt.text,
                        "next_node": opt.next_node,
                        "condition": opt.condition,
                        "effects": opt.effects,
                        "hidden": opt.hidden
                    }
                    for opt in node.options
                ]
            }
            
            if node.timeout:
                nodes_json[node_id]["timeout"] = node.timeout
            if node.auto_advance:
                nodes_json[node_id]["auto_advance"] = True
            if node.next_node:
                nodes_json[node_id]["next_node"] = node.next_node
        
        return {
            "dialogue_id": dialogue.dialogue_id,
            "version": dialogue.version,
            "meta": dialogue.meta,
            "entry_point": dialogue.entry_point,
            "nodes": nodes_json,
            "variables": dialogue.variables
        }


def main():
    parser = argparse.ArgumentParser(
        description='Convert Markdown dialogue files to JSON for UE5'
    )
    parser.add_argument('input', type=Path, help='Input .md file or directory')
    parser.add_argument('output', type=Path, help='Output .json file or directory')
    parser.add_argument('--batch', action='store_true', help='Batch mode (process directory)')
    
    args = parser.parse_args()
    
    converter = DialogueConverter()
    
    if args.batch or args.input.is_dir():
        converter.convert_batch(args.input, args.output)
    else:
        converter.convert(args.input, args.output)


if __name__ == '__main__':
    main()
